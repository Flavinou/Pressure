#include <Pressure.h>
#include <Pressure/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Pressure
{

	class Sigil : public Application 
	{
	public:
		Sigil(ApplicationCommandLineArgs args)
			: Application("Sigil", args)
		{
			PushLayer(new EditorLayer());
		}

		~Sigil()
		{
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new Sigil(args);
	}

}
