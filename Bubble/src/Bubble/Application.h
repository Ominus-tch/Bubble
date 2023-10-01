#pragma once

#include "Core.h"

#include "Window.h"
#include "Bubble/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Bubble/ImGui/ImGuiLayer.h"

#include "Bubble/Renderer/Shader.h"
#include "Bubble/Renderer/VertexArray.h"

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
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		LayerStack m_LayerStack;

		unsigned int m_VertexArrayInt;
		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<VertexBuffer> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	private:
		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();
}


