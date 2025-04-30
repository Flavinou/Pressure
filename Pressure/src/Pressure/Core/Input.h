#pragma once

#include "Pressure/Core/KeyCodes.h"
#include "Pressure/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace Pressure
{

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
	};

}
