#include "Listner.h"
#include <iostream>

namespace ash
{
	void AcceptTask::BeginAccept()
	{
		_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		char buf[1024];
		DWORD dwRecv;
		if (!::AcceptEx(_listener._listenSocket, _sock, buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwRecv, &overlapped))
		{
			auto err = ::WSAGetLastError();
			if (err != ERROR_IO_PENDING)
			{
				printf("acceptex error\n");
			}
		}
	}

	void AcceptTask::OnAccept()
	{
		printf("socket connected\n");
		_cb(_sock, 0);

		BeginAccept();
	}
	
	void AcceptTask::Run()
	{
		OnAccept();
	}

	Listener::Listener(IoWorker & iow, std::function<void(SOCKET, int)> cb)
		: _iow(iow)
	{
		_acceptTask = new AcceptTask(*this, cb);
		_acceptTask->IsStatic = true;
	}

	void Listener::Listen(short port)
	{
		_listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (_listenSocket == INVALID_SOCKET)
		{
			throw new std::exception();
		}

		::CreateIoCompletionPort((HANDLE)_listenSocket, _iow.Raw(), (ULONG_PTR)0, 0);

		// bind
		SOCKADDR_IN serveraddr;
		::memset(&serveraddr, 0, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(port);
		serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
		if (SOCKET_ERROR == ::bind(_listenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)))
		{
			throw new std::exception();
		}

		// listen
		if (SOCKET_ERROR == ::listen(_listenSocket, SOMAXCONN))
		{
			throw new std::exception();
		}

		Accept();
	}

	// cb: callback

	void Listener::Accept()
	{
		_acceptTask->BeginAccept();
	}
}