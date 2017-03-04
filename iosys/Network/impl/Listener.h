#pragma once

#include <exception>
#include "WindowsHeader.h"
#include <functional>
#include "IoService/IoService.h"
#include "Session.h"
#include <vector>

namespace ash
{
	typedef std::function<void(SOCKET, int /*err*/)> AcceptCbT;

	class Listener
	{
	public:
		Listener(NetworkService& iow, AcceptCbT cb, int acceptCount);
		void Listen(short port);

		NetworkService& GetService()
		{
			return _iow;
		}

	private:
		void Accept();

		NetworkService& _iow;
		SOCKET _listenSocket{ INVALID_SOCKET };
		std::vector<SPtr<class AcceptTask>> _acceptTasks;

		friend class AcceptTask;
	};

	class AcceptTask : public NetworkTaskBase
	{
	public:
		AcceptTask(Listener& listener, AcceptCbT cb, bool isStatic, int num)
			: NetworkTaskBase(isStatic), _listener(listener), _cb(cb), _num(num)
		{}

		void BeginAccept();
		virtual void Run(DWORD) override;

	private:
		void OnAccept();

		char _buf[1024]{ 0, };
		DWORD _dwRecv{ 0 };
		SOCKET _sock{ INVALID_SOCKET };
		AcceptCbT _cb;
		Listener& _listener;
		int _num;
	};
}