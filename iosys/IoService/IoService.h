#pragma once

#include "WindowsHeader.h"
#include "memory.h"

namespace ash
{
	struct OverlappedExt : public OVERLAPPED
	{
		class NetworkTaskBase* task;
	};

	class NetworkTaskBase
	{
	public:
		NetworkTaskBase(bool isStatic)
			: IsStatic(isStatic)
		{
			::memset(&overlapped, 0, sizeof(OverlappedExt));
			overlapped.task = this;
		}

		virtual void Run(DWORD recvdBytes)
		{}

		OverlappedExt overlapped;
		const bool IsStatic{ false };
	};

	class NetworkService
	{
	public:
		NetworkService();
		~NetworkService();
		void Run()
		{
			ProcessIo();
		}

		HANDLE Raw()
		{
			return _iocp;
		}

	private:
		void ProcessIo();

	private:
		HANDLE _iocp{ INVALID_HANDLE_VALUE };
	};
}