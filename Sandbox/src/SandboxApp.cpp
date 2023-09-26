#include <Bubble.h>

class ExampleLayer : public Bubble::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		BG_INFO("ExampleLayer::OnUpdate");
	}

	void OnEvent(Bubble::Event& event) override
	{
		BG_TRACE("{0}", event);
	}
};

class Sandbox : public Bubble::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Bubble::ImGuiLayer());
	}

	~Sandbox()
	{
 
	}
};

Bubble::Application* Bubble::CreateApplication()
{
	return new Sandbox();
}