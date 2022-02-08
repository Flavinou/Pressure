#include <Pressure.h>

#include "imgui/imgui.h"

class ExampleLayer : public Pressure::Layer
{
public:
	Pressure::OrthographicCamera& m_MainCamera;

	ExampleLayer()
		:Layer("Example"), m_MainCamera((&Pressure::Application::Get())->GetMainCamera())
	{
	}

	void OnUpdate() override
	{
		if (Pressure::Input::IsKeyPressed(PRS_KEY_TAB))
			PRS_TRACE("Tab key is pressed ! (poll)");
	}

	void OnImGuiRender()
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}

	void OnEvent(Pressure::Event& event) override
	{
		if (event.GetEventType() == Pressure::EventType::KeyPressed)
		{
			Pressure::KeyPressedEvent& e = (Pressure::KeyPressedEvent&)event;
			if (e.GetKeyCode() == PRS_KEY_TAB)
				PRS_TRACE("Tab key is pressed ! (event)");
			PRS_TRACE("{0}", (char)e.GetKeyCode());

			switch (e.GetKeyCode())
			{
			case PRS_KEY_LEFT:
				m_MainCamera.SetPosition({ m_MainCamera.GetPosition().x + 0.05f, m_MainCamera.GetPosition().y, 0.0f });
				break;
			case PRS_KEY_RIGHT:
				m_MainCamera.SetPosition({ m_MainCamera.GetPosition().x - 0.05f, m_MainCamera.GetPosition().y, 0.0f });
				break;
			case PRS_KEY_UP:
				m_MainCamera.SetPosition({ m_MainCamera.GetPosition().x, m_MainCamera.GetPosition().y - 0.05f, 0.0f });
				break;
			case PRS_KEY_DOWN:
				m_MainCamera.SetPosition({ m_MainCamera.GetPosition().x, m_MainCamera.GetPosition().y + 0.05f, 0.0f });
				break;
			}
		}
	}
};

class Sandbox : public Pressure::Application 
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Pressure::Application* Pressure::CreateApplication()
{
	return new Sandbox();
}