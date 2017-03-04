#include "IoService/IoService.h"
#include "Network/Network.h"
#include "Network\impl\Buffer.h"
#include "Message\MessageSerializer.h"

using namespace ash;

class TestMsg
{
public:
	int i;
	std::string name;

	template<class Arc>
	void serialize(Arc& arc)
	{
		arc(i, name);
	}
};

class TestSessionHandler : public ash::ISessionHandler
{
public:
	virtual void ProcessMessage(SPtr<SessionBase> session, Buffer& buf, DWORD msgLength) override
	{
		printf("testsessionhandler!\n");

		MessageSerializer ms;
		DWORD processed;
		TestMsg msg;

		ms.Deserialize(buf.Raw + 2, buf.Offset - 2, processed, msg);
		if (msgLength != processed + 2)
		{
			throw new std::exception();
		}

		printf("msg: {%d, %s}\n", msg.i, msg.name.c_str());
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