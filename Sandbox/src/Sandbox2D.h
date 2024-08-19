#pragma once

#include <Bubble.h>

#include <map>

class EditorLayer : public Bubble::Layer
{
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Bubble::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Bubble::Event& e) override;

private:
	//Cell* GetLowestEntropyCell();
	//void ResetGrid();
private:
	Bubble::OrthographicCameraController m_CameraController;

	Bubble::Ref<Bubble::Texture2D> m_CheckerboardTexture;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	float m_FPS = 0.f;
};