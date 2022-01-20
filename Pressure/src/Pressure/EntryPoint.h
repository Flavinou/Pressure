#pragma once

#ifdef PRS_PLATFORM_WINDOWS

extern Pressure::Application* Pressure::CreateApplication();

int main(int argc, char** argv)
{
	Pressure::Log::Init();
	PRS_CORE_WARN("Initialized Logging !");
	PRS_CLIENT_INFO("Initialized Logging !");

	auto app = Pressure::CreateApplication();
	app->Run();
	delete app;
}

#endif // PRS_PLATFORM_WINDOWS