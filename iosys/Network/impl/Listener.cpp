#include "Listener.h"
#include <iostream>
#include "Session.h"

namespace ash
{
	Listener::Listener(NetworkService & iow, AcceptCbT cb, int acceptCount)
		: _iow(iow)
	{
		for (int i = 0; i < acceptCount; ++i)
		{
			_acceptTasks.emplace_back(NewShared<AcceptTask>(*this, cb, true, i));
		}
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

	void Listener::Accept()
	{
		for (auto task : _acceptTasks)
		{
			task->BeginAccept();
		}
	}
	
	void AcceptTask::BeginAccept()
	{
		_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (!::AcceptEx(_listener._listenSocket, _sock, _buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &_dwRecv, &overlapped))
		{
			auto err = ::WSAGetLastError();
			if (err != ERROR_IO_PENDING)
			{
				printf("acceptex error\n");
			}
		}
	}

	void AcceptTask::Run(DWORD)
	{
		OnAccept();
	}

	void AcceptTask::OnAccept()
	{
		printf("acceptTask(%d), dwRecv(%d), socket connected\n", _num, _dwRecv);
		
		_cb(_sock, 0);
		_sock = INVALID_SOCKET;

		BeginAccept();
	}
}