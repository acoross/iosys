#pragma once

#include <exception>
#include "WindowsHeader.h"
#include <functional>
#include "IoService/IoService.h"
#include "Session.h"
#include <vector>

namespace ash
{
	typedef std::function<void(SPtr<Session>, int /*err*/)> AcceptCbT;

	class Listener
	{
	public:
		Listener(IoWorker& iow, AcceptCbT cb, int acceptCount);
		void Listen(short port);

	private:
		void Accept();

		IoWorker& _iow;
		SOCKET _listenSocket{ INVALID_SOCKET };
		std::vector<SPtr<class AcceptTask>> _acceptTasks;

		friend class AcceptTask;
	};

	class AcceptTask : public IOverlappedTask
	{
	public:
		AcceptTask(Listener& listener, AcceptCbT cb, bool isStatic)
			: _listener(listener), _cb(cb)
		{
			IsStatic = isStatic;
		}

		void BeginAccept();
		virtual void Run() override;

	private:
		void OnAccept();

		SOCKET _sock{ INVALID_SOCKET };
		AcceptCbT _cb;
		Listener& _listener;
	};
}