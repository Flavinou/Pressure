#pragma once

#include "Pressure/Core/Base.h"

#ifdef PRS_PLATFORM_WINDOWS

extern Pressure::Application* Pressure::CreateApplication();

int main(int argc, char** argv)
{
	Pressure::Log::Init();
	PRS_CORE_INFO("Initialized Logging.");

	PRS_PROFILE_BEGIN_SESSION("Startup", "PressureProfile-Startup.json");
	auto app = Pressure::CreateApplication();
    PRS_PROFILE_END_SESSION();

	PRS_PROFILE_BEGIN_SESSION("Runtime", "PressureProfile-Runtime.json");
	app->Run();
	PRS_PROFILE_END_SESSION();

	PRS_PROFILE_BEGIN_SESSION("Shutdown", "PressureProfile-Shutdown.json");
	delete app;
	PRS_PROFILE_END_SESSION();
}

#endif // PRS_PLATFORM_WINDOWS