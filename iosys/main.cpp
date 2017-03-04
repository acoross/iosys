#include "IoService/IoService.h"
#include "Network/Network.h"

int main()
{
	ash::IoWorker iow;
	ash::Network::Initialize();

	ash::Listener listener(iow, [&](ash::SPtr<ash::Session> session, int error)
	{
		printf("error %d\n", error);
	}, 10);

	listener.Listen(7777);
	iow.Run();

	ash::Network::Finalize();

	return 0;
}