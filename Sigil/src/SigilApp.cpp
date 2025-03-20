#include <Pressure.h>
#include <Pressure/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Pressure
{

	class Sigil : public Application 
	{
	public:
		Sigil()
			: Application("Sigil")
		{
			PushLayer(new EditorLayer());
		}

		~Sigil()
		{
		}
	};

	Application* CreateApplication()
	{
		return new Sigil();
	}

}
