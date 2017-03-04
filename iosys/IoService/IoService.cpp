#include "IoService.h"
#include "WindowsHeader.h"

namespace ash
{
	NetworkService::NetworkService()
	{
		_iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
	}

	NetworkService::~NetworkService()
	{
		if (_iocp != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(_iocp);
			_iocp = INVALID_HANDLE_VALUE;
		}
	}
	
	void NetworkService::ProcessIo()
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
				auto* overlappedExt = reinterpret_cast<OverlappedExt*>(overlapped);
				overlappedExt->task->Run(dwNumBytes);
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