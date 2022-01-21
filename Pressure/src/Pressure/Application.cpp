#include "prspch.h"
#include "Application.h"

#include "Pressure/Events/ApplicationEvent.h"
#include "Pressure/Log.h"

namespace Pressure
{

	Application::Application()
	{
	}
	
	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			PRS_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			PRS_TRACE(e);
		}


		while (true);
	}

}
