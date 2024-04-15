#include <Bubble.h>
#include "imgui/imgui.h"

class ExampleLayer : public Bubble::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
		BG_INFO("Hello");
	}

	void OnUpdate() override
	{
		if (Bubble::Input::IsKeyPressed(BG_KEY_TAB))
			BG_TRACE("Tab key is pressed");
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World!");
		ImGui::End();
	}

	void OnEvent(Bubble::Event& event) override
	{
		auto eventType = event.GetEventType();
		if (eventType == Bubble::EventType::KeyPressed)
		{
			Bubble::KeyPressedEvent& e = (Bubble::KeyPressedEvent&)event;
			BG_TRACE("{0} ({1})", (char)e.GetKeyCode(), e.GetKeyCode());
		}
		else if (eventType == Bubble::EventType::MouseButtonPressed)
		{
			Bubble::MouseButtonEvent& e = (Bubble::MouseButtonEvent&)event;

			POINT cursorPos = {};
			GetCursorPos(&cursorPos);

			BG_TRACE("Pressed: {0} at ({1}, {2})", e.GetMouseButton(), cursorPos.x, cursorPos.y);
		}
	}
};

class Sandbox : public Bubble::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{
 
	}
};

Bubble::Application* Bubble::CreateApplication()
{
	return new Sandbox();
}