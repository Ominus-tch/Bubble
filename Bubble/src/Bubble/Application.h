#pragma once

#include "Core.h"

#include "Window.h"
#include "Bubble/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Bubble
{
	class BUBBLE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void OnExit();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	private:
		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();
}


