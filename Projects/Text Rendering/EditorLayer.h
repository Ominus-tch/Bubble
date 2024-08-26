#pragma once

#include <Bubble.h>

#include <span>
#include "Font.h"

namespace Bubble {

	static void DrawGlyph(GlyphData glyph)
	{
		int contourStartIndex = 0;

		for (int contourEndIndex : glyph.contourEndIndices)
		{
			int numPointsInContour = contourEndIndex - contourStartIndex + 1;
			std::span<Point> points(glyph.Points.data() + contourStartIndex, numPointsInContour);

			for (int i = 0; i < points.size(); i++)
			{
				//BG_INFO("{0}, {1} -> {2}, {3}", points[i].x, points[i].y, points[(i + 1) % points.size()].x, points[(i + 1) % points.size()].y);
				const auto& p1 = points[i];
				const auto& p2 = points[(i + 1) % points.size()];
				Renderer2D::DrawLine(
					{ p1.x, p1.y, 1.f },
					{ p2.x, p2.y, 1.f },
					{ 1.f, 1.f, 1.f, 1.f });
			}

			contourStartIndex = contourEndIndex + 1;
		}

		for (int i = 0; i < glyph.Points.size(); i++)
		{
			Renderer2D::DrawCircleFilled({ glyph.Points[i].x, glyph.Points[i].y, 1.f}, 10.f, {1.f, 1.f, 1.f, 1.f});
		}
	}

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

		GlyphData glyph0;
	};

}
