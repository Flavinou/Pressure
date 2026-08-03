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

	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			PRS_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "Pressure Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
	public:
		Application(ApplicationSpecification&& specification);
		virtual ~Application();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		static Application& Get() { return *s_Instance; }
		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

		Window& GetWindow() const { return *m_Window; }
		ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; }

		float GetSpeed() const { return m_TimeScale; }
		void SetSpeed(const float value)
		{
			if (value < 0.0f)
			{
				PRS_CORE_WARN("Application::SetSpeed() - Speed cannot be negative. Value: {0}", value);
			}
			else if (value > 2.0f)
			{
				PRS_CORE_WARN("Application::SetSpeed() - Speed cannot be greater than 2. Value: {0}", value);
			}
			else
			{
				m_TimeScale = value;
			}
		}

		void Close();

		void SubmitToMainThread(const std::function<void()>& function);
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

		void ExecuteMainThreadQueue();

        static void ResetStats();
	private:
		ApplicationSpecification m_Specification;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;

		Timestep m_Timestep;
		float m_LastFrameTime = 0.0f;
		float m_TimeScale = 1.0f;
		uint32_t m_FrameId = 0;

		std::vector<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadQueueMutex;
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in client application
	Application* CreateApplication(ApplicationCommandLineArgs args);

}

