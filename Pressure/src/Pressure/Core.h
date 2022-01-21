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

#define BIT(x) (1 << x)