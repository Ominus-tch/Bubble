#pragma once

#include "Bubble/Layer.h"

#include "Bubble/Events/KeyEvent.h"
#include "Bubble/Events/MouseEvent.h"
#include "Bubble/Events/ApplicationEvent.h"

namespace Bubble {

	class BUBBLE_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
			
		void OnAttach();
		void OnDetach();
		void OnUpdate();
		void OnEvent(Event& event);
	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool OnMouseMovedEvent(MouseMovedEvent& e);
		bool OnMouseScrolledEvent(MouseScrolledEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnKeyReleasedEvent(KeyReleasedEvent& e);
		bool OnKeyTypedEvent(KeyTypedEvent& e);
		bool OnWindowResizedEvent(WindowResizeEvent& e);
	private:
		float m_Time = 0.f;
	};

}