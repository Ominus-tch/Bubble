#include <Bubble.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "imgui/imgui.h"

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
	View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
	View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
	return Projection * View * Model;
}

class ExampleLayer : public Bubble::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{

		auto cam = camera(5.0f, { 0.5f, 0.5f });
		
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