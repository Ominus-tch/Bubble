#pragma once

#ifdef BG_PLATFORM_WINDOWS

extern Bubble::Application* Bubble::CreateApplication();

int main(int argc, char** argv)
{
	Bubble::Application* app = Bubble::CreateApplication();
	app->Run();
	delete app;
}

#endif