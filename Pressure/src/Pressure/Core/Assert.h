#pragma once

#include "Pressure/Core/Base.h"
#include "Pressure/Core/Log.h"

// TODO: Make this macro able to take in no argument except condition
#ifdef PRS_ENABLE_ASSERTS

	namespace Pressure::Assert
	{
		// Returns the simple file name rather than full path
		constexpr const char* CurrentFileName(const char* path)
		{
			const char* file = path;
			while (*path)
			{
				if (*path == '/' || *path == '\\')
					file = ++path;
				else
					path++;
			}
			return file;
		}

	}

	// Alternatively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatted string instead of having the format inside the default message
	#define PRS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { PRS##type##ERROR(msg, __VA_ARGS__); PRS_DEBUGBREAK(); } }
	#define PRS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) PRS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define PRS_INTERNAL_ASSERT_NO_MSG(type, check) PRS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", PRS_STRINGIFY_MACRO(check), ::Pressure::Assert::CurrentFileName(__FILE__), __LINE__)

	#define PRS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define PRS_INTERNAL_ASSERT_GET_MACRO(...) PRS_EXPAND_MACRO( PRS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, PRS_INTERNAL_ASSERT_WITH_MSG, PRS_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define PRS_ASSERT(...) PRS_EXPAND_MACRO( PRS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define PRS_CORE_ASSERT(...) PRS_EXPAND_MACRO( PRS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define PRS_ASSERT(x, ...)
	#define PRS_CORE_ASSERT(x, ...)
#endif // PRS_ENABLE_ASSERTS
