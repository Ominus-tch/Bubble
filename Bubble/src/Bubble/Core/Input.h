#pragma once

#include "Bubble/Core/MouseCodes.h"
#include "Bubble/Core/KeyCodes.h"

#include <glm/glm.hpp>

namespace Bubble {

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