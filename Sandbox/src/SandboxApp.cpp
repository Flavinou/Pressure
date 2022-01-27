#include <Pressure.h>

class ExampleLayer : public Pressure::Layer
{
public:
	ExampleLayer()
		:Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Pressure::Input::IsKeyPressed(PRS_KEY_TAB))
			PRS_TRACE("Tab key is pressed ! (poll)");
	}

	void OnEvent(Pressure::Event& event) override
	{
		if (event.GetEventType() == Pressure::EventType::KeyPressed)
		{
			Pressure::KeyPressedEvent& e = (Pressure::KeyPressedEvent&)event;
			if (e.GetKeyCode() == PRS_KEY_TAB)
				PRS_TRACE("Tab key is pressed ! (event)");
			PRS_TRACE("{0}", (char)e.GetKeyCode());
		}
	}
};

class Sandbox : public Pressure::Application 
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Pressure::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

Pressure::Application* Pressure::CreateApplication()
{
	return new Sandbox();
}