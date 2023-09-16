#pragma once

#include "Core.h"

namespace Bubble
{
	class BUBBLE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	// To be defined in client
	Application* CreateApplication();
}


