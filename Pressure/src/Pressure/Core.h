#pragma once

#ifdef PRS_PLATFORM_WINDOWS
	#ifdef PRS_BUILD_DLL
		#define PRESSURE_API __declspec(dllexport)
	#else
		#define	PRESSURE_API __declspec(dllimport)
	#endif // PRS_BUILD_DLL
#else
	#error Pressure only supports Windows for now !
#endif // PRS_PLATFORM_WINDOWS

#ifdef PRS_ENABLE_ASSERTS
	#define PRS_ASSERT(x, ...) { if(!(x)) { PRS_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define PRS_CORE_ASSERT(x, ...) { if(!(x)) { PRS_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define PRS_ASSERT(x, ...)
	#define PRS_CORE_ASSERT(x, ...)
#endif // PRS_ENABLE_ASSERTS

#define BIT(x) (1 << x)