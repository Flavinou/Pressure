#pragma once

#include "Core.h"
#include "Events/Event.h"

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

