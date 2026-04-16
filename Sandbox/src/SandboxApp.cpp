#include <Pressure.h>
#include <Pressure/Core/EntryPoint.h>

#include "Sandbox2D.h"

class Sandbox : public Pressure::Application 
{
public:
	explicit Sandbox(Pressure::ApplicationSpecification&& spec)
		: Application(std::move(spec))
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox() override = default;
};

Pressure::Application* Pressure::CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../Sigil";
	spec.CommandLineArgs = args;

	return new Sandbox(std::move(spec));
}
