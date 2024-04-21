#include "ExampleLayer.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer()
	: Layer("ExampleLayer"), m_CameraController(1280.0f / 720.0f)
{

}

void ExampleLayer::OnAttach()
{
}

void ExampleLayer::OnDetach()
{
}

void ExampleLayer::OnUpdate(Bubble::Timestep ts)
{
}

void ExampleLayer::OnImGuiRender()
{
}

void ExampleLayer::OnEvent(Bubble::Event& e)
{
	m_CameraController.OnEvent(e);
}
