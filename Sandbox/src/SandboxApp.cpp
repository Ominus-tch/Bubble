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
			BG_INFO("Tab key is pressed");
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World!");
		ImGui::End();
	}

	void OnEvent(Bubble::Event& event) override
	{
		if (event.GetEventType() == Bubble::EventType::KeyPressed)
		{
			Bubble::KeyPressedEvent& e = (Bubble::KeyPressedEvent&)event;
			BG_TRACE("{0} ({1})", (char)e.GetKeyCode(), e.GetKeyCode());
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