#pragma once

#include "Core.h"

#include "Window.h"
#include "Bubble/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

#include "Bubble/ImGui/ImGuiLayer.h"

#include "Bubble/Renderer/Renderer.h"
#include "Bubble/Renderer/Shader.h"
#include "Bubble/Renderer/VertexArray.h"
#include "Bubble/Renderer/OrthographicCamera.h"

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

		std::shared_ptr<Shader> m_Shader;
		std::shared_ptr<VertexArray> m_VertexArray;

		std::shared_ptr<Shader> m_BlueShader;
		std::shared_ptr<VertexArray> m_SquareVA;

		OrthographicCamera m_Camera;
		float rotation = 0.f;
	private:
		static Application* s_Instance;
	};

	// To be defined in client
	Application* CreateApplication();
}
