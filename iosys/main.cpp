#include "IoService/IoService.h"
#include "Network/Network.h"
#include "Network\impl\Buffer.h"
#include "Message\MessageSerializer.h"

#include "ClientToTestServer.h"

using namespace ash;

class TestSessionHandler
	: public ash::ISessionHandler, 
	public Test::ClientToTestServer
{
public:
	TestSessionHandler()
	{
		_handlers.push_back([&](Buffer& buf, DWORD msgLength)
		{
			MessageSerializer ms;
			DWORD processed;
			Test::TestMsg msg;
			ms.Deserialize(buf.Raw + 2, buf.Offset - 2, processed, msg);
			if (msgLength != processed + 2)
			{
				throw new std::exception();
			}

			Process(msg);
		});
	}

	virtual void ProcessMessage(SPtr<SessionBase> session, Buffer& buf, DWORD msgLength) override
	{
		printf("testsessionhandler!\n");

		MessageSerializer ms;
		DWORD processed;
		
		Test::TestMsg msg;

		ms.Deserialize(buf.Raw + 2, buf.Offset - 2, processed, msg);
		if (msgLength != processed + 2)
		{
			throw new std::exception();
		}

		printf("msg: {%d, %s}\n", msg.i, msg.name.c_str());
	}

	std::vector<std::function<void(Buffer&, DWORD)>> _handlers;

	// ClientToTestServer을(를) 통해 상속됨
	virtual void Process(Test::TestMsg& msg) override
	{
	}

	// ClientToTestServer을(를) 통해 상속됨
	virtual void Process(Test::TestMsg2 & msg) override
	{
	}
};

typedef ash::Session<TestSessionHandler> TestSession;

int main()
{
	ash::NetworkService iow;
	ash::Network::Initialize();

	ash::Listener listener(iow, [&](SOCKET sock, int error)
	{
		auto session = ash::NewShared<TestSession>(sock, iow);
		session->Read();
	}, 10);

	listener.Listen(7777);
	iow.Run();

	ash::Network::Finalize();

	return 0;
}