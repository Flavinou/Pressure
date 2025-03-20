#include <Pressure.h>
#include <Pressure/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Pressure
{

	class PressureEditor : public Application 
	{
	public:
		PressureEditor()
			: Application("Pressure Editor")
		{
			PushLayer(new EditorLayer());
		}

		~PressureEditor()
		{
		}
	};

	Application* CreateApplication()
	{
		return new PressureEditor();
	}

}
