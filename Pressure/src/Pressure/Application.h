#pragma once

#include "Core.h"

#include "Window.h"
#include "Pressure/LayerStack.h"
#include "Pressure/Events/Event.h"
#include "Pressure/Events/ApplicationEvent.h"

#include "Pressure/ImGui/ImGuiLayer.h"

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

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	// To be defined in client application
	Application* CreateApplication();

}

