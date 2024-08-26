#pragma once

#include <Bubble.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Bubble {
	class Particle {

	public:

		Particle(glm::vec2 pos = { 0.f, 0.f }, float r = 1.f)
		{
			m_Pos = pos;
			m_R = r;

			m_Scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_R, m_R, 1.0f));

			m_Vel = { 0.f, 0.f };
		}

		void Update(Timestep ts)
		{
			m_Pos.x += m_Vel.x * ts;
			m_Pos.y += m_Vel.y * ts;
		}

		void Draw() const
		{
			Renderer2D::DrawCircle(GetTransform(), glm::vec4(0.f, 0.f, 0.f, 1.f));
		}

		glm::vec2 GetPos() const { return m_Pos; }
		void SetPos(glm::vec2 pos) { m_Pos = pos; }
		glm::mat4 GetTransform() const
		{
			auto translation = glm::translate(glm::mat4(1.0f), { m_Pos.x, m_Pos.y, 1.f });
			auto transform = translation * m_Scale;
			return glm::mat4(transform);
		}

		glm::vec2 GetVel() const { return m_Vel; }
		void SetVel(glm::vec2 vel) { m_Vel = vel; }

	private:
		glm::mat4 m_Scale;

		float m_R;

		glm::vec2 m_Vel;
		glm::vec2 m_Pos;
	};
}