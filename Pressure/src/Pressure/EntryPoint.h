#pragma once

#ifdef PRS_PLATFORM_WINDOWS

extern Pressure::Application* Pressure::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Pressure::CreateApplication();
	app->Run();
	delete app;
}

#endif // PRS_PLATFORM_WINDOWS