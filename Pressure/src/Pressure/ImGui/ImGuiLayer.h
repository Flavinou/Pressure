#pragma once

#include "Pressure/Layer.h"

#include "Pressure/Events/ApplicationEvent.h"
#include "Pressure/Events/KeyEvent.h"
#include "Pressure/Events/MouseEvent.h"

namespace Pressure
{

	class PRESSURE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}