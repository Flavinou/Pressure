#pragma once

#include "Pressure/Core/Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Pressure
{

	class Log
	{
	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define PRS_CORE_CRITICAL(...)	  ::Pressure::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define PRS_CORE_ERROR(...)		  ::Pressure::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PRS_CORE_WARN(...)		  ::Pressure::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PRS_CORE_INFO(...)		  ::Pressure::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PRS_CORE_TRACE(...)		  ::Pressure::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Client log macros				  
#define PRS_CRITICAL(...)		  ::Pressure::Log::GetClientLogger()->critical(__VA_ARGS__)
#define PRS_ERROR(...)			  ::Pressure::Log::GetClientLogger()->error(__VA_ARGS__)
#define PRS_WARN(...)			  ::Pressure::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PRS_INFO(...)			  ::Pressure::Log::GetClientLogger()->info(__VA_ARGS__)
#define PRS_TRACE(...)			  ::Pressure::Log::GetClientLogger()->trace(__VA_ARGS__)