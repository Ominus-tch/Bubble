#pragma once

#include "Bubble/Core/Base.h"
#include "Bubble/Core/Application.h"

#ifdef BG_PLATFORM_WINDOWS

extern Bubble::Application* Bubble::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Bubble::Log::Init();

	BG_PROFILE_BEGIN_SESSION("Startup", "BubbleProfiler-Startup.json");
	Bubble::Application* app = Bubble::CreateApplication({argc, argv});
	BG_PROFILE_END_SESSION();

	BG_PROFILE_BEGIN_SESSION("Runtime", "BubbleProfiler-Runtime.json");
	app->Run();
	BG_PROFILE_END_SESSION();

	BG_PROFILE_BEGIN_SESSION("Shutdown", "BubbleProfiler-Shutdown.json");
	delete app;
	BG_PROFILE_END_SESSION();
}

#endif
