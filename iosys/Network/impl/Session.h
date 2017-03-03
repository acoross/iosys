#pragma once

#include <stdio.h>
#include "WindowsHeader.h"

namespace ash
{
	// socket
	class Session
	{
	public:
		Session(SOCKET sock)
			: _sock(sock)
		{}

		void Send()
		{}

	private:
		void ProcessRead()
		{

		}

		SOCKET _sock{ INVALID_SOCKET };
	};
}