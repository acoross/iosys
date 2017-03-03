#include "IoService.h"
#include "WindowsHeader.h"

namespace ash
{
	IoWorker::IoWorker()
	{
		_iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	}

	IoWorker::~IoWorker()
	{
		if (_iocp != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(_iocp);
			_iocp = INVALID_HANDLE_VALUE;
		}
	}
	
	void IoWorker::Run()
	{
		for (;;)
		{
			DWORD dwNumBytes;
			ULONG_PTR completionKey;
			OVERLAPPED* overlapped{ nullptr };
			auto ret = ::GetQueuedCompletionStatus(_iocp, &dwNumBytes, &completionKey, &overlapped, 0);
			DWORD dwError = ::GetLastError();

			if (ret)
			{
				auto* overlappedExt = static_cast<OverlappedExt*>(overlapped);
				overlappedExt->task->Run();
				if (!overlappedExt->task->IsStatic)
				{
					delete overlapped;
				}
			}
			else
			{
				if (dwError != WAIT_TIMEOUT)
				{
					break;
				}
			}
		}
	}
}