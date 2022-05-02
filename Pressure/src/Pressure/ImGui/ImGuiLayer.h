#pragma once

#include "Pressure/Core/Layer.h"

#include "Pressure/Events/ApplicationEvent.h"
#include "Pressure/Events/KeyEvent.h"
#include "Pressure/Events/MouseEvent.h"

namespace Pressure
{

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}