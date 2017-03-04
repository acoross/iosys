#pragma once

#include <exception>

namespace ash
{
	class Buffer
	{
	public:
		DWORD Offset{ 0 };
		char Raw[1024]{ 0, };

		char* GetEmpty()
		{
			return Raw + Offset;
		}

		DWORD EmptyLength()
		{
			return 1024 - Offset;
		}

		void Clear(DWORD length)
		{
			if (Offset < length)
			{
				throw new std::exception();
			}

			Offset -= length;
		}
	};
}