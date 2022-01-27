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
		PRS_INFO("ExampleLayer::Update");
	}

	void OnEvent(Pressure::Event& event) override
	{
		PRS_TRACE("{0}", event);
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