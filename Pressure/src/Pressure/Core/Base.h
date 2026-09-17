#pragma once

#include "Pressure/Core/PlatformDetection.h"

#include <memory>

#if defined(PRS_PLATFORM_WINDOWS)
	#define PRS_DEBUGBREAK() __debugbreak()
#elif defined(PRS_PLATFORM_LINUX)
	#include <signal.h>
	#define PRS_DEBUGBREAK() raise(SIGTRAP)
#else
	#error "Platform doesn't support debugbreak yet!"
#endif

#if defined(PRS_DEBUG)
	#define PRS_ENABLE_ASSERTS
#endif

#if !defined(PRS_DIST)
	#define PRS_ENABLE_VERIFY
#endif

#define PRS_EXPAND_MACRO(x) x
#define PRS_STRINGIFY_MACRO(x) #x

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

#include "Pressure/Core/Log.h"
#include "Pressure/Core/Assert.h"
