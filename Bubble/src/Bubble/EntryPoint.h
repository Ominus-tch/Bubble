#pragma once

#ifdef BG_PLATFORM_WINDOWS

extern Bubble::Application* Bubble::CreateApplication();

int main(int argc, char** argv)
{
	Bubble::Log::Init();
	BG_CORE_WARN("Initialized Log!");
	int a = 5;
	BG_DEBUG("Hello! var={0}", a);

	Bubble::Application* app = Bubble::CreateApplication();
	app->Run();
	delete app;
}

#endif