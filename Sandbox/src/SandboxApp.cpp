#include <Bubble.h>
#include <Bubble/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "ExampleLayer.h"

class Sandbox : public Bubble::Application
{
public:
	Sandbox(const Bubble::ApplicationSpecification& specification)
	: Bubble::Application(specification)
	{
		//Bubble::Window& window = GetWindow();
		//window.SetVSync(false);

		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{
 
	}
};

Bubble::Application* Bubble::CreateApplication(Bubble::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	//spec.WorkingDirectory = "../Hazelnut";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}