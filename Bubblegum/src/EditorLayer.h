#pragma once

#include <Bubble.h>

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include <deque>


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
		glm::vec2 m_MouseLastClicked;

		Entity m_CameraEntity;
		bool m_PrimaryCamera = true;

		EditorCamera m_EditorCamera;

		bool m_ViewportFocused = false, m_ViewportHovered = false;
		glm::vec2 m_ViewportSize = { 0.f, 0.f };
		glm::vec2 m_ViewportBounds[2];

		// App

		ShaderLibrary m_ShaderLib;

		Ref<Shader> m_Shader;
		Ref<Texture2D> m_OutputTexture;
		Ref<Texture2D> m_TestTexture;

		Ref<ComputeBuffer> m_DestinationTexture;

		struct RayTracingMaterial
		{
			glm::vec4 colour;
			glm::vec4 emissionColour;
			glm::vec4 specularColour;
			float emissionStrength;
			float smoothness;
			float specularProbability;
			int flag;
		};

		struct Sphere
		{
			glm::vec3 position;
			float radius;
			RayTracingMaterial material;
		};
		std::vector<Sphere> m_Spheres;
		Ref<ComputeBuffer> m_SphereBuffer;

		Ref<Texture2D> Test;

		struct CameraData
		{
			glm::vec4 u_CameraPosition;
			glm::vec4 u_ViewParams;
			glm::mat4 u_CameraToWorld;
		};
		CameraData m_CameraData;
		Ref<UniformBuffer> m_CameraBuffer;

		struct EnvironmentSettings
		{
			glm::vec4 GroundColour = glm::vec4(0.3, 0.2, 0.1, 1.0);
			glm::vec4 SkyColourHorizon = glm::vec4(0.7, 0.9, 1.0, 1.0);
			glm::vec4 SkyColourZenith = glm::vec4(0.2, 0.4, 0.8, 1.0);
			int EnvironmentEnabled = 1;
			float SunFocus = 32.f;
			float SunIntensity = 5.f;
		};
		EnvironmentSettings m_EnvironmentSettings;
		Ref<UniformBuffer> m_EnvironmentBuffer;

		void DrawMaterialControls(RayTracingMaterial& mat);

		struct WorldData
		{
			int MaxBounceCount = 1;
			int NumRaysPerPixel = 1;
			int Frame = 0;
			int NumSpheres;
			glm::ivec2 PixelCount;
		};
		WorldData m_WorldData;
		Ref<UniformBuffer> m_WorldDataBuffer;

		struct QuadVert {
			glm::vec3 Position;
			glm::vec2 UV;
			int EntityID;
		};

		struct ShaderData
		{
			glm::vec4 Resolution;
			glm::vec4 Mouse;
			float Time;
			int Frame;
		};
		ShaderData m_ShaderData;
		Ref<UniformBuffer> m_ShaderDataBuffer;

		const QuadVert s_QuadVertices[6] = {
			// Positions          // UVs
			{ {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}, -1 },  // Top-left
			{ {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, -1 },  // Bottom-left
			{ { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, -1 },  // Bottom-right

			{ {-1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}, -1 },  // Top-left
			{ { 1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, -1 },  // Bottom-right
			{ { 1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, -1 }   // Top-right
		};

		Entity m_ControlPoint1;
		Entity m_ControlPoint2;
		Entity m_ControlPoint3;
		Entity m_ControlPoint4;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<VertexArray> m_VertexArray;

		// End app

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
