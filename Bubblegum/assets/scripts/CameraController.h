#pragma once

#include "Bubble.h"

inline glm::vec3 mul(glm::vec3 a, float b) { return glm::vec3(a.x * b, a.y * b, a.z * b); }

namespace Bubble {

    class CameraController : public ScriptableEntity
    {
    public:
        virtual void OnCreate() override
        {

        }

        virtual void OnDestroy() override
        {

        }

		virtual void OnUpdate(Timestep ts) override
		{
			if (!HasComponent<CameraComponent>())
				return;

			const auto& camera = GetComponent<CameraComponent>();

			if (camera.Primary)
			{
				switch (camera.Camera.GetProjectionType())
				{
					case SceneCamera::ProjectionType::Orthographic: {
						if (Input::IsKeyPressed(Key::LeftShift))
							return;

						auto& transform = GetComponent<TransformComponent>();
						float speed = 5.f;

						if (Input::IsKeyPressed(Key::W))
							transform.Translation.y += speed * ts;
						if (Input::IsKeyPressed(Key::S))
							transform.Translation.y -= speed * ts;

						if (Input::IsKeyPressed(Key::A))
							transform.Translation.x -= speed * ts;
						if (Input::IsKeyPressed(Key::D))
							transform.Translation.x += speed * ts;

						if (Input::IsKeyPressed(Key::Q))
							transform.Rotation.z -= speed * ts;
						if (Input::IsKeyPressed(Key::E))
							transform.Rotation.z += speed * ts;
						break;
					}
					case SceneCamera::ProjectionType::Perspective: {
						float mouseX = Input::GetMouseX();
						float mouseY = Input::GetMouseY();
						if (Input::IsKeyPressed(Key::LeftShift))
						{
							lastMousePos = { mouseX, mouseY };
							return;
						}

						auto& transform = GetComponent<TransformComponent>();
						float speed = 10.f;

						glm::vec3 forward, up, right;
						std::tie(forward, up, right) = transform.GetDirectionVectors();

						if (Input::IsKeyPressed(Key::W))
							transform.Translation -= mul(forward, speed * ts);
						if (Input::IsKeyPressed(Key::S))
							transform.Translation += mul(forward, speed * ts);

						if (Input::IsKeyPressed(Key::A))
							transform.Translation -= mul(right, speed * ts);
						if (Input::IsKeyPressed(Key::D))
							transform.Translation += mul(right, speed * ts);

						if (Input::IsKeyPressed(Key::Q))
							transform.Translation -= mul(up, speed * ts);
						if (Input::IsKeyPressed(Key::E))
							transform.Translation += mul(up, speed * ts);
						

						// Mouse

						

						float mouseSens = 0.4f;

						if (lastMousePos == glm::vec2(-1.f))
						{
							lastMousePos = { mouseX, mouseY };
							return;
						}

						if (Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
							glm::vec2 mouseDelta = glm::vec2(mouseX, mouseY) - lastMousePos;

							// Update yaw (Y-axis rotation) and pitch (X-axis rotation)
							transform.Rotation.z -= mouseDelta.x * mouseSens * ts;  // Yaw
							transform.Rotation.x -= mouseDelta.y * mouseSens * ts;  // Pitch

							// Clamp the pitch to prevent flipping (e.g., between -89.9 and 89.9 degrees)
							transform.Rotation.x = glm::clamp(transform.Rotation.x, 0.01f, glm::pi<float>() - 0.01f);
						}

						lastMousePos = { mouseX, mouseY };

						break;
					}
				}
				
			}
		}
	private:
		glm::vec2 lastMousePos = glm::vec2(-1.f);
    };
}
