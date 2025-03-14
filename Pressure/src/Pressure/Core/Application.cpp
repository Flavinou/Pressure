#include "prspch.h"
#include "Pressure/Core/Application.h"

#include "Pressure/Core/Log.h"
#include "Pressure/Renderer/Renderer.h"

#include "Pressure/Core/Input.h"

#include <GLFW/glfw3.h>

namespace Pressure
{

	Application* Application::s_Instance = nullptr;

	static Application::Statistics s_Stats;

	Application::Application()
	{
		PRS_CORE_ASSERT(!s_Instance, "Application already exists !");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetEventCallback(PRS_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		PRS_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		PRS_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		PRS_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

    void Application::ResetStats()
    {
        memset(&s_Stats, 0, sizeof(Application::Statistics));
    }

    Application::Statistics Application::GetStats()
    {
        return s_Stats;
    }

    void Application::OnEvent(Event& e)
	{
		PRS_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(PRS_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(PRS_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			if (e.Handled)
				break;
			(*--it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		PRS_PROFILE_FUNCTION();

		while (m_Running)
		{
			PRS_PROFILE_SCOPE("RunLoop");

			m_FrameId++;

			float time = (float)glfwGetTime(); // Platform::GetTime
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			// Engine stats every 25th frame
			if (m_FrameId % 25 == 0) 
			{
				ResetStats();
				s_Stats.FrameTime = timestep.GetMilliseconds();
				s_Stats.FramesPerSecond = GetFramesPerSecond(timestep);
			}

			if (!m_Minimized)
			{
				{
					PRS_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					PRS_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		PRS_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		PRS_CORE_TRACE("{0}, {1}", e.GetWidth(), e.GetHeight());

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

}
