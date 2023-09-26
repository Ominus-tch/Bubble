#pragma once

#include "Bubble/Core.h"
#include "MouseButtonCodes.h"
#include "KeyCodes.h"

namespace Bubble {

	class BUBBLE_API Input
	{
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }
		inline static float GetMouseX() { return s_Instance->GetMousePosImpl().first; }
		inline static float GetMouseY() { return s_Instance->GetMousePosImpl().second; }
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;

		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;
	private:
		static Input* s_Instance;
	};

}