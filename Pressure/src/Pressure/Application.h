#pragma once

#include "Core.h"

#include "Window.h"
#include "Pressure/LayerStack.h"
#include "Pressure/Events/Event.h"
#include "Pressure/Events/ApplicationEvent.h"

namespace Pressure
{

	class PRESSURE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	// To be defined in client application
	Application* CreateApplication();

}

