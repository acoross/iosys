#include "IoService/IoService.h"
#include "Network/Network.h"

int main()
{
	ash::IoWorker iow;
	ash::Network::Initialize();

	ash::Listener listener(iow, [&](SOCKET s, int error)
	{
		printf("error %d\n", error);
	});

	listener.Listen(7777);
	iow.Run();

	ash::Network::Finalize();

	return 0;
}