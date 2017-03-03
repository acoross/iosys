#pragma once

#include <exception>
#include "WindowsHeader.h"
#include <functional>
#include "IoService/IoService.h"

namespace ash
{
	class Listener
	{
	public:
		Listener(IoWorker& iow, std::function<void(SOCKET, int /*err*/)> cb);
		void Listen(short port);

	private:
		void Accept();

		IoWorker& _iow;
		SOCKET _listenSocket{ INVALID_SOCKET };
		class AcceptTask* _acceptTask;

		friend class AcceptTask;
	};

	class AcceptTask : public IOverlappedTask
	{
	public:
		AcceptTask(Listener& listener, std::function<void(SOCKET, int /*err*/)> cb)
			: _listener(listener), _cb(cb)
		{}

		void BeginAccept();
		virtual void Run() override;

	private:
		void OnAccept();

		SOCKET _sock{ INVALID_SOCKET };
		std::function<void(SOCKET, int /*err*/)> _cb;
		Listener& _listener;
	};
}