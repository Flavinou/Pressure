#pragma once

#include "Core.h"

#include "Window.h"

#include "Pressure/Core/LayerStack.h"

#include "Pressure/Events/Event.h"
#include "Pressure/Events/ApplicationEvent.h"

#include "Pressure/Core/Timestep.h"

#include "Pressure/ImGui/ImGuiLayer.h"

namespace Pressure
{

	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;
	private:
		static Application* s_Instance;
	};

	// To be defined in client application
	Application* CreateApplication();

}

