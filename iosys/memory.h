#pragma once

#include <memory>

namespace ash
{
	template<typename T>
	using SPtr = std::shared_ptr<T>;

	template<typename T, typename... Args>
	inline SPtr<T> NewShared(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}