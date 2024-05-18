#pragma once

#include <Bubble.h>

namespace Bubble {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		void OnRender(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		OrthographicCameraController m_CameraController;


		glm::vec2 m_ViewportSize = { 0.f, 0.f };

		float m_LastFrameTime = 0.f;
		float m_FPS = 0.f;
		uint32_t m_FrameCount = 0;
	
		float m_LoopTime = 0.f;

		Ref<Framebuffer> m_Framebuffer;

		// App
		Ref<Texture2D> m_CheckerboardTexture;
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	};

}
