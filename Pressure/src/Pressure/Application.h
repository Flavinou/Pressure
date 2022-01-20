#pragma once

#include "Core.h"

namespace Pressure
{

	class PRESSURE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in client application
	Application* CreateApplication();

}

