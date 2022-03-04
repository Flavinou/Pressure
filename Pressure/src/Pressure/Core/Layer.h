#pragma once

#include "Pressure/Core/Core.h"
#include "Pressure/Core/Timestep.h"
#include "Pressure/Events/Event.h"

namespace Pressure
{

	class PRESSURE_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
	
}

