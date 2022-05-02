#pragma once

#include <memory>

#ifdef PRS_DEBUG
	#define PRS_ENABLE_ASSERTS
#endif // PRS_DEBUG

#ifdef PRS_ENABLE_ASSERTS
	#define PRS_ASSERT(x, ...) { if(!(x)) { PRS_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define PRS_CORE_ASSERT(x, ...) { if(!(x)) { PRS_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define PRS_ASSERT(x, ...)
	#define PRS_CORE_ASSERT(x, ...)
#endif // PRS_ENABLE_ASSERTS

#define BIT(x) (1 << x)

#define PRS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Pressure
{

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}