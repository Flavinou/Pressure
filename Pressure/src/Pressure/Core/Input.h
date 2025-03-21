#pragma once

#include "Pressure/Core/Base.h"
#include "Pressure/Core/KeyCodes.h"
#include "Pressure/Core/MouseCodes.h"

namespace Pressure
{

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
	};

}