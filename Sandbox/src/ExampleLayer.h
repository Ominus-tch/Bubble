#pragma once

#include <Bubble.h>

class ExampleLayer : public Bubble::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Bubble::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Bubble::Event& e) override;
private:
	Bubble::ShaderLibrary m_ShaderLibrary;
	Bubble::OrthographicCameraController m_CameraController;
};