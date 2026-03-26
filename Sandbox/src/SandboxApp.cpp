#include <Pressure.h>
#include <Pressure/Core/EntryPoint.h>

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sandbox2D.h"

class Sandbox : public Pressure::Application 
{
public:
	Sandbox(Pressure::ApplicationCommandLineArgs args)
		: Application("Sandbox", args)
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
	}
};

Pressure::Application* Pressure::CreateApplication(ApplicationCommandLineArgs args)
{
	return new Sandbox(args);
}
