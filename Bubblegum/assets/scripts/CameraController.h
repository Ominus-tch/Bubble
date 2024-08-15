#pragma once

#include "Bubble.h"

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
                auto& transform = GetComponent<TransformComponent>();
                float speed = 5.f;

                if (Input::IsKeyPressed(Key::A))
                    transform.Translation.x -= speed * ts;
                if (Input::IsKeyPressed(Key::D))
                    transform.Translation.x += speed * ts;
                if (Input::IsKeyPressed(Key::W))
                    transform.Translation.y += speed * ts;
                if (Input::IsKeyPressed(Key::S))
                    transform.Translation.y -= speed * ts;
            }
        }
    };
}