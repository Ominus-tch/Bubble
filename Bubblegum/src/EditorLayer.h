#pragma once

#include <Bubble.h>

#include "Panels/SceneHierarchyPanel.h"

#include <filesystem>

namespace Bubble {

	class Segment
	{
	public:
		Segment(glm::vec3 a, glm::vec3 b, glm::vec3 origin, bool animate = false)
		{
			m_A = a;
			m_B = b;

			m_StartA = a;
			m_StartB = b;

			m_Origin = origin;

			if (!animate)
			{
				m_Angle = 90.f;
				m_Completed = true;

				glm::vec3 va = m_StartA - m_Origin;
				glm::vec3 vb = m_StartB - m_Origin;

				float angle = glm::radians(m_Angle);
				glm::vec3 axis(0.0f, 0.0f, 1.0f);

				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
				va = glm::vec3(rotationMatrix * glm::vec4(va, 1.0f));
				vb = glm::vec3(rotationMatrix * glm::vec4(vb, 1.0f));

				m_A = m_Origin + va;
				m_B = m_Origin + vb;
			}
		}

		void Draw() const
		{
			Renderer2D::DrawLine(m_A, m_B, { 1.f, 1.f, 1.f, 1.f });
		}

		void Update(Timestep ts)
		{
			if (!m_Completed)
			{
				m_Angle += ts * 90.f; // 2 seconds (90 / 45)
				BG_INFO(m_Angle);
				if (m_Angle >= 90.f)
				{
					m_Angle = 90.f;
					m_Completed = true;
				}

				glm::vec3 va = m_StartA - m_Origin;
				glm::vec3 vb = m_StartB - m_Origin;

				float angle = glm::radians(m_Angle);
				glm::vec3 axis(0.0f, 0.0f, 1.0f);

				glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
				va = glm::vec3(rotationMatrix * glm::vec4(va, 1.0f));
				vb = glm::vec3(rotationMatrix * glm::vec4(vb, 1.0f));

				m_A = m_Origin + va;
				m_B = m_Origin + vb;
			}
		}

		glm::vec3 A() const { return m_A; }
		glm::vec3 B() const { return m_B; }

		bool m_Completed = false;
	private:
		glm::vec3 m_StartA, m_StartB;
		glm::vec3 m_A, m_B;
		glm::vec3 m_Origin;

		float m_Angle = 0.f;
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

		void Iteration();

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseScroll(MouseScrolledEvent& e);

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
	private:
		glm::vec2 m_ViewportSize = { 0.f, 0.f };

		float m_FPS = 0.f;

		Ref<Framebuffer> m_Framebuffer;

		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		Entity m_SquareEntity;
		Entity m_Square2;

		Entity m_CameraEntity;
		Entity m_SecondCamera;
		bool m_PrimaryCamera = true;

		// App
		std::vector<Segment*> m_Segments;
		Segment* m_EndSegment;
		bool m_Animate = true;

		Ref<Texture2D> m_CheckerboardTexture;
		uint32_t m_FrameCount = 0;
		float m_TimeSinceLastRender = 0.f;
		float m_Time = 0.f;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};
		SceneState m_SceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;

	};

}
