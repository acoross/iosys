#pragma once

#include <string>

namespace Test
{
	class TestMsg
	{
	public:
		int i;
		std::string name;

		template<class Arc>
		void serialize(Arc& arc)
		{
			arc(i, name);
		}
	};

	struct TestMsg2
	{
		int a;
		std::string name;

		template<class Arc>
		void serialize(Arc& arc)
		{
			arc(a, name);
		}
	};

	class ClientToTestServer
	{
	public:
		virtual void Process(TestMsg& msg) = 0;
		virtual void Process(TestMsg2& msg) = 0;
	};
}