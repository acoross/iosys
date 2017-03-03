#pragma once

#include "WindowsHeader.h"
#include "memory.h"

namespace ash
{
	struct OverlappedExt : public OVERLAPPED
	{
		class IOverlappedTask* task;
	};

	class IOverlappedTask
	{
	public:
		IOverlappedTask()
		{
			::memset(&overlapped, 0, sizeof(OverlappedExt));
			overlapped.task = this;
		}

		virtual void Run() = 0;

		OverlappedExt overlapped;
		bool IsStatic{ false };
	};

	class IoWorker
	{
	public:
		IoWorker();
		~IoWorker();
		void Run();
		HANDLE Raw()
		{
			return _iocp;
		}

	private:
		HANDLE _iocp{ INVALID_HANDLE_VALUE };
	};
}