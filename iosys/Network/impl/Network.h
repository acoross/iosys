#pragma once

#include "WindowsHeader.h"
#include <exception>
#include "Listner.h"

namespace ash
{
	class Network
	{
	public:
		static void Initialize()
		{
			WSADATA wsaData;
			if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				throw new std::exception();
			}
		}

		static void Finalize()
		{
			::WSACleanup();
		}
	};
}