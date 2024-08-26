#pragma once

#include <Bubble.h>

#include <glm/gtc/matrix_transform.hpp>

constexpr uint32_t c = 30;
constexpr double G = 2;

namespace Bubble {

	namespace Utils {

		inline void SetMag(glm::vec2& vec, float magnitude) { 
			vec = glm::normalize(vec) * magnitude;
		}

		inline void LimitVec(glm::vec2& vec, float limitMagnitude) {
			float magnitude = glm::length(vec);

			if (magnitude > limitMagnitude) {
				vec = glm::normalize(vec) * limitMagnitude;
			}
		}

		inline void LimitVec(glm::vec2& vec, const glm::vec2 limitVec) {
			LimitVec(vec, glm::length(limitVec));
		}


	}

	class Photon
	{
	public:
		Photon(glm::vec2 pos = { 0.f, 0.f })
			: m_Pos(pos), m_Vel({ -(static_cast<float>(c))/* * 5000.f*/, 0 })
		{

		}

		void Update(Timestep ts)
		{
			const glm::vec2 start = m_Pos;

			m_Pos.x += m_Vel.x * ts;
			m_Pos.y += m_Vel.y * ts;

			m_History.insert(m_History.begin(), { start, m_Pos });

			if (m_History.size() > 1000)
			{
				m_History.pop_back();
			}
		}

		glm::mat4 GetTransform()
		{
			return glm::translate(glm::mat4(1.0f), { m_Pos.x, m_Pos.y, 1.f });
		}

		glm::vec2 GetPos() const { return m_Pos; }
		glm::vec2& GetPosRef() { return m_Pos; }
		void SetPos(glm::vec2 newPos) { m_Pos = newPos; }

		glm::vec2 GetVel() const { return m_Vel; }
		glm::vec2& GetVelRef() { return m_Vel; }
		void SetVel(glm::vec2 newVel) { m_Vel = newVel; }

		std::vector<std::pair<glm::vec2, glm::vec2>>* GetHistory() { return &m_History; }

	private:
		glm::vec2 m_Pos, m_Vel;
		std::vector<std::pair<glm::vec2, glm::vec2>> m_History;
	};

	class Blackhole 
	{
	public:
		Blackhole(glm::vec2 pos = { 0.f, 0.f }, float mass = 3000.f)
			: m_Pos(pos), m_Mass(mass), m_RS((2.f * G * m_Mass) / (c * c))
		{
			BG_INFO("RS: {0}", m_RS);
		}

		void Init()
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), { m_Pos.x, m_Pos.y, 0.f });

			m_BlackholeTransform = translation * glm::scale(glm::mat4(1.0f), glm::vec3(m_RS, m_RS, 1.0f));
			m_PhotonRingTransform = translation * glm::scale(glm::mat4(1.0f), glm::vec3(m_RS * 3.f + 32.f, m_RS * 3.f + 32.f, 1.0f));
			m_AccresionDiskTransform = translation * glm::scale(glm::mat4(1.0f), glm::vec3(m_RS * 1.5f + 16.f, m_RS * 1.5f + 16.f, 1.0f));
		}

		void Draw(OrthographicCameraController& camera)
		{
			Renderer2D::BeginScene(camera.GetCamera());
			
			Renderer2D::DrawCircle(m_BlackholeTransform, glm::vec4(0.f, 0.f, 0.f, 1.f));
			Renderer2D::DrawCircle(m_PhotonRingTransform, glm::vec4(0.392f, 0.392f, 0.392f, 0.392f), 0.2f);
			Renderer2D::DrawCircle(m_AccresionDiskTransform, glm::vec4(1.f, 0.588f, 0.f, 1.f), 0.15f);

			Renderer2D::EndScene();
		}

		void Pull(Photon* photon)
		{
			glm::vec2 force = (m_Pos - photon->GetPos());
			const float r = glm::length(force);
			const float fg = G * (m_Mass * 0.05f) / (r * r);

			Utils::SetMag(force, fg);
			glm::vec2& vel = photon->GetVelRef();
			vel.x += force.x;
			vel.y += force.y;
			Utils::SetMag(vel, c);
		}

		glm::vec2 GetPos() const { return m_Pos; }
		float GetMass() const { return m_Mass; }
		float GetRS() const { return m_RS; }

	private:
		glm::vec2 m_Pos;
		float m_Mass;
		float m_RS;

		glm::mat4 m_BlackholeTransform = {};
		glm::mat4 m_PhotonRingTransform = {};
		glm::mat4 m_AccresionDiskTransform = {};
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

		float m_LastFrameTime = 0.f;
		float m_FPS = 0.f;
		uint32_t m_FrameCount = 0;
	
		float m_LoopTime = 0.f;

		Ref<Framebuffer> m_Framebuffer;

		// App
		Ref<Texture2D> m_CheckerboardTexture;
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

		Blackhole m_Blackhole;
		std::vector<Photon*> m_Photons;
	};

}
