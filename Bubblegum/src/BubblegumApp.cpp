#include <Bubble.h>
#include <Bubble/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Bubble {

	class Bubblegum : public Application
	{
	public:
		Bubblegum(const ApplicationSpecification& specification)
		: Application(specification)
		{
			//Window& window = GetWindow();
			//window.SetVSync(false);

			//PushLayer(new ExampleLayer());
			PushLayer(new EditorLayer());
		}

		~Bubblegum()
		{
 
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "Bubblegum";
		//spec.WorkingDirectory = "../Bubblegum";
		spec.CommandLineArgs = args;

		return new Bubblegum(spec);
	}

}
