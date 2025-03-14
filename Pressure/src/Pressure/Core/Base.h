#pragma once

#include <memory>

#ifdef PRS_DEBUG
	#if defined(PRS_PLATFORM_WINDOWS)
		#define PRS_DEBUGBREAK() __debugbreak()
	#elif defined(PRS_PLATFORM_LINUX)
		#include <signal.h>
		#define PRS_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define PRS_ENABLE_ASSERTS
#else
	#define PRS_DEBUGBREAK()
#endif // PRS_DEBUG

#ifdef PRS_ENABLE_ASSERTS
	#define PRS_ASSERT(x, ...) { if(!(x)) { PRS_ERROR("Assertion failed: {0}", __VA_ARGS__); PRS_DEBUGBREAK(); } }
	#define PRS_CORE_ASSERT(x, ...) { if(!(x)) { PRS_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); PRS_DEBUGBREAK(); } }
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