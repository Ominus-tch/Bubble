#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

EditorLayer::EditorLayer()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{
}

void EditorLayer::OnAttach()
{
	BG_PROFILE_FUNCTION()

	m_CheckerboardTexture = Bubble::Texture2D::Create("assets/textures/Checkerboard.png");
}

void EditorLayer::OnDetach()
{
	BG_PROFILE_FUNCTION()
}

void EditorLayer::OnUpdate(Bubble::Timestep ts)
{
	BG_PROFILE_FUNCTION()

	m_FPS = 1.f / ts;

    Bubble::Renderer2D::ResetStats();
    {
        BG_PROFILE_SCOPE("Renderer Prep");
        Bubble::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
        Bubble::RenderCommand::Clear();
    }

    {
        BG_PROFILE_SCOPE("Renderer Draw")

            static float rotation = 0.0f;
        rotation += ts * 50.0f;

        Bubble::Renderer2D::BeginScene(m_CameraController.GetCamera());
        Bubble::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
        Bubble::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
        Bubble::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
        Bubble::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
        Bubble::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, glm::radians(rotation), m_CheckerboardTexture, 20.0f);
        Bubble::Renderer2D::EndScene();

        Bubble::Renderer2D::BeginScene(m_CameraController.GetCamera());
        for (float y = -5.0f; y < 5.0f; y += 0.5f)
        {
            for (float x = -5.0f; x < 5.0f; x += 0.5f)
            {
                glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
                Bubble::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
            }
        }
        Bubble::Renderer2D::EndScene();
    }

	// Update
	m_CameraController.OnUpdate(ts);
}

void EditorLayer::OnImGuiRender()
{
    BG_PROFILE_FUNCTION()

    ImGui::Begin("Settings");

    auto stats = Bubble::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::Text("FPS: %f", m_FPS);

    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    
    //uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
    //ImGui::Image((void*)textureID, ImVec2(m_Framebuffer->GetSpecification().Width, m_Framebuffer->GetSpecification().Height));
    //ImGui::Image((void*)m_CheckerboardTexture->GetRendererID(), ImVec2(256.f, 256.f));
    ImGui::End();
}

void EditorLayer::OnEvent(Bubble::Event& e)
{
	m_CameraController.OnEvent(e);
}