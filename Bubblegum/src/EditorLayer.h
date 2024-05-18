#pragma once

#include <Bubble.h>

namespace Bubble {

	namespace Utils {
		inline glm::vec2 ScreenToWorld(glm::vec2 mouseScreenCoords, glm::vec2 screenDims, glm::vec2 cameraPosition, float cameraZoom) {

			// Convert to NDC
			glm::vec2 ndc;
			ndc.x = (mouseScreenCoords.x / screenDims.x) * 2.0f - 1.0f;
			ndc.y = 1.f - (mouseScreenCoords.y / screenDims.y) * 2.0f;

			// Convert to world coordinates
			glm::vec2 worldCoords;
			worldCoords.x = (ndc.x * (screenDims.x / 2.0f)) / cameraZoom + cameraPosition.x;
			worldCoords.y = (ndc.y * (screenDims.y / 2.0f)) / cameraZoom + cameraPosition.y;

			return worldCoords;
		}
	}

	class Boundary
	{
	public:
		Boundary(glm::vec2 a, glm::vec2 b)
			: m_PosA(a), m_PosB(b)
		{

		}

		glm::vec2 GetPosA() const { return m_PosA; }
		glm::vec2 GetPosB() const { return m_PosB; }

	private:
		glm::vec2 m_PosA, m_PosB;
	};

	class Ray
	{
	public:
		Ray()
		{

		}


	private:
	};

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
		bool m_ViewportFocused = false;
		bool m_ViewportHovered = false;

		float m_LastFrameTime = 0.f;
		float m_FPS = 0.f;
		uint32_t m_FrameCount = 0;
	
		float m_LoopTime = 0.f;

		Ref<Framebuffer> m_Framebuffer;

		// App
		Ref<Texture2D> m_CheckerboardTexture;
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		std::vector<Boundary> m_Boundries;
		glm::vec2 m_MouseWorldPos = {};
		glm::vec2 m_WindowPos = {};
	};

}
