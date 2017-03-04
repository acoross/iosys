#pragma once

#include <stdio.h>
#include <memory>
#include "WindowsHeader.h"
#include "IoService\IoService.h"
#include "Buffer.h"

namespace ash
{
	class SessionBase : public std::enable_shared_from_this<SessionBase>
	{
	public:
		virtual void Send() {};
	};

	template <typename SessionHandlerT>
	class Session : public SessionBase
	{
	public:
		typedef Session<SessionHandlerT> SessionT;

		Session(SOCKET sock, NetworkService& iow)
			: _sock(sock), _iow(iow)
		{
			::CreateIoCompletionPort((HANDLE)_sock, _iow.Raw(), (ULONG_PTR)0, 1);
			_handler = NewShared<SessionHandlerT>();
		}

		void Read()
		{
			_recvTask = NewShared<class RecvTask>(shared_from_this());
			_recvTask->BeginRecv();
		}

		// SessionBase을(를) 통해 상속됨
		virtual void Send() override
		{
		}

	private:
		class RecvTask : public NetworkTaskBase
		{
		public:
			RecvTask(SPtr<SessionBase> session)
				: NetworkTaskBase(true), _session(session)
			{
			}
			void BeginRecv()
			{
				_wsabuf.buf = _recvBuffer.GetEmpty();
				_wsabuf.len = _recvBuffer.EmptyLength();
				DWORD flag = 0;
				
				auto* session = static_cast<SessionT*>(_session.get());
				if (0 != ::WSARecv(session->_sock, &_wsabuf, 1, &_recvBytes, &flag, &overlapped, nullptr))
				{
					auto err = ::WSAGetLastError();
					if (err != WSA_IO_PENDING)
					{
						throw new std::exception();
					}
				}
			}

			// IOverlappedTask을(를) 통해 상속됨
			virtual void Run(DWORD recvdBytes) override
			{
				OnRecv(recvdBytes);
			}

		private:
			void OnRecv(DWORD recvdBytes)
			{
				if (recvdBytes > 0)
				{
					_recvBuffer.Offset += recvdBytes;

					for (;;)
					{
						if (_recvBuffer.Offset < 2)
						{
							return;
						}

						auto length = *(reinterpret_cast<unsigned short*>(_recvBuffer.Raw));
						if (_recvBuffer.Offset < length)
						{
							return;
						}

						printf("length: %d\n", length);
						auto* session = static_cast<SessionT*>(_session.get());
						session->_handler->ProcessMessage(_session, _recvBuffer, length);
						_recvBuffer.Clear(length);
					}
				}

				//
				BeginRecv();
			}

			SPtr<SessionBase> _session;
			Buffer _recvBuffer;
			DWORD _recvBytes{ 0 };
			WSABUF _wsabuf;
		};

		SOCKET _sock{ INVALID_SOCKET };
		NetworkService& _iow;
		SPtr<class RecvTask> _recvTask;
		SPtr<SessionHandlerT> _handler;
	};

	class ISessionHandler
	{
	public:
		virtual void ProcessMessage(SPtr<SessionBase> session, Buffer& buf, DWORD msgLength) = 0;
	};
}

#include "Session.inc"