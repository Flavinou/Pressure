#include <Pressure.h>
#include <Pressure/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Pressure
{

	class Sigil : public Application 
	{
	public:
		explicit Sigil(ApplicationSpecification&& spec)
			: Application(std::move(spec))
		{
			PushLayer(new EditorLayer());
		}

		~Sigil() override = default;
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Sigil";
		spec.CommandLineArgs = args;

		return new Sigil(std::move(spec));
	}

}
