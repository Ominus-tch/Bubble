#include "bgpch.h"
#include "Bubble/Core/Window.h"

#ifdef BG_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Bubble
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef BG_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		BG_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}
