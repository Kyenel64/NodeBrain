#pragma once

namespace NodeBrain
{
	template<typename T, typename U>
	constexpr std::shared_ptr<T> CastPtr(U ptr)
	{
		return std::static_pointer_cast<T>(ptr);
	}
}