#pragma once

#include <Bubble.h>

#include "Panels/SceneHierarchyPanel.h"

#include <filesystem>

namespace Bubble {

	struct fourier
	{
		float re;
		float im;
		size_t freq;
		float amp;
		float phase;
	};

	static std::vector<fourier> dft(std::vector<float> x) {
		std::vector<fourier> X;
		size_t N = x.size();
		X.reserve(N);
		for (size_t k = 0; k < N; k++) {
			float re = 0.f;
			float im = 0.f;
			for (size_t n = 0; n < N; n++) {
				float phi = (PI * 2.f * k * n) / N;
				re += x[n] * cosf(phi);
				im -= x[n] * sinf(phi);
			}
			re = re / N;
			im = im / N;

			size_t freq = k;
			float amp = sqrtf(re * re + im * im);
			float phase = atan2f(im, re);
			X[k] = { re, im, freq, amp, phase };
		}
		return X;
	}

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

		glm::vec3 EpiCycles(glm::vec3 pos, float rotation, std::vector<fourier> fourier);

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
		std::vector<fourier> m_FourierX;
		std::vector<fourier> m_FourierY;

		std::vector<float> m_X;
		std::vector<float> m_Y;

		std::vector<glm::vec2> m_Path;

		std::vector<float> m_Wave;

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
