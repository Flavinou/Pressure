#pragma once

#include <memory>
#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Pressure
{

	class PRESSURE_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};

}

// Core log macros
#define PRS_CORE_FATAL(...)		  ::Pressure::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define PRS_CORE_ERROR(...)		  ::Pressure::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PRS_CORE_WARN(...)		  ::Pressure::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PRS_CORE_INFO(...)		  ::Pressure::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PRS_CORE_TRACE(...)		  ::Pressure::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Core log macros				  
#define PRS_CLIENT_FATAL(...)	  ::Pressure::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define PRS_CLIENT_ERROR(...)	  ::Pressure::Log::GetClientLogger()->error(__VA_ARGS__)
#define PRS_CLIENT_WARN(...)	  ::Pressure::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PRS_CLIENT_INFO(...)	  ::Pressure::Log::GetClientLogger()->info(__VA_ARGS__)
#define PRS_CLIENT_TRACE(...)	  ::Pressure::Log::GetClientLogger()->trace(__VA_ARGS__)