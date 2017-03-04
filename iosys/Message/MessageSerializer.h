#pragma once
#include <cereal\archives\binary.hpp>
#include <cereal\types\string.hpp>
#include <cereal\types\array.hpp>
#include <cereal\types\vector.hpp>
#include <strstream>

namespace ash
{
	class MessageSerializer
	{
	public:
		template <typename Msg>
		void Deserialize(const char* buf, DWORD length, DWORD& processedLength, Msg& outMsg)
		{
			std::istrstream ss(buf, length);
			cereal::BinaryInputArchive arc(ss);
			arc(outMsg);
			processedLength = ss.str() - buf;
		}

		template <typename Msg>
		void Serialize(Msg& msg, char* buf, DWORD length, DWORD& processedLength)
		{
			std::ostrstream ss(buf, length);
			cereal::BinaryOutputArchive arc(ss);
			arc(msg);
			processedLength = (DWORD)ss.pcount();
		}
	};
}