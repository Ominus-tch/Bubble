#pragma once

#include <Bubble.h>

#include "Panels/SceneHierarchyPanel.h"

namespace Bubble {

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		glm::vec2 m_ViewportSize = { 0.f, 0.f };

		float m_FPS = 0.f;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Entity m_SquareEntity;
		Entity m_Square2;

		Entity m_CameraEntity;
		Entity m_SecondCamera;
		bool m_PrimaryCamera = true;

		// App
		Ref<Texture2D> m_CheckerboardTexture;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};

}
