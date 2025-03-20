#pragma once

#include "Pressure/Core/Base.h"

#include "Pressure/Core/Window.h"

#include "Pressure/Core/LayerStack.h"

#include "Pressure/Events/Event.h"
#include "Pressure/Events/ApplicationEvent.h"

#include "Pressure/Core/Timestep.h"

#include "Pressure/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Pressure
{

	class Application
	{
	public:
		Application(const std::string& name = "Pressure App");
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

		void Close();

		inline static Application& Get() { return *s_Instance; }
	public:
        // Statistics
        struct Statistics
        {
			float FrameTime = 0.0f;
            float FramesPerSecond = 0.0f;
        };

        static Statistics GetStats();
		static float GetFramesPerSecond(Timestep ts) { return 1 / ts; }
	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
        static void ResetStats();
	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;
		uint32_t m_FrameId = 0;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in client application
	Application* CreateApplication();

}

