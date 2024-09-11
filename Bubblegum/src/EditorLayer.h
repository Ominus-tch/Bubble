#pragma once

#include <Bubble.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include <deque>


namespace Bubble {

	class CelestialBody
	{
	public:
		CelestialBody(const glm::vec3& pos, const glm::vec3& vel, float r)
			: m_Pos(pos), m_Vel(vel), m_R(r)
		{

		}

		void Draw()
		{
			Renderer2D::DrawSphere(m_Pos, m_R);
		}

	private:
		glm::vec3 m_Pos;
		glm::vec3 m_Vel;
		float m_R;
	};

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
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseScroll(MouseScrolledEvent& e);

		void OnOverlayRender();

		void NewProject();
		bool OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void SaveProject();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();
		void OnScenePause();

		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();
	private:

		float m_FPS = 0.f;
		std::deque<float> m_FPSCache;
		float m_FPSSum = 0.0f;
		const size_t MaxCacheSize = 250;

		float m_FPSCap = 240.f;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

		Entity m_HoveredEntity;

		Entity m_CameraEntity;
		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.f, 0.f };
		glm::vec2 m_ViewportBounds[2];

		// App


		ShaderLibrary m_ShaderLib;
		Ref<Shader> m_Shader;

		Ref<ComputeBuffer> m_Buffer;

		Entity m_MeshTest;
		Entity m_Cube;

		bool m_Animate = true;

		Ref<Texture2D> m_CheckerboardTexture;
		uint32_t m_FrameCount = 0;
		float m_TimeSinceLastRender = 0.f;
		float m_Time = 0.f;

		int m_GizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		Scope<ContentBrowserPanel> m_ContentBrowserPanel;

		// Editor resources
		Ref<Texture2D> m_IconPlay, m_IconPause, m_IconStep, m_IconSimulate, m_IconStop;
	};

}
