#include "EditorLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define DEG2RAD(deg) deg * (PI / 180.f)

namespace Bubble {

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
    {
    }

    void EditorLayer::OnAttach()
    {
        BG_PROFILE_FUNCTION();

        m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

        FramebufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        fbSpec.Width = 1280; fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_CameraController.SetZoomLevel(10.f);

    }

    void EditorLayer::OnDetach()
    {
        BG_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        BG_PROFILE_FUNCTION();

        d += 150.f * ts;

        /*if (m_FrameCount < 10)
        {
            m_FrameCount++;
            Random::Next(ts);
        }*/

        //m_LoopTime += ts;

        m_FPS = 1.f / ts;

        // Update
        m_CameraController.OnUpdate(ts);
    }

    void EditorLayer::OnRender(Timestep ts)
    {
        Renderer2D::ResetStats();
        {
            BG_PROFILE_SCOPE("Renderer Prep");
            m_Framebuffer->Bind();
            RenderCommand::SetClearColor({ 1.f, 1.f, 1.f, 1 });
            //RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
            RenderCommand::Clear();
        }

        {
            BG_PROFILE_SCOPE("Renderer Draw");

            Renderer2D::BeginScene(m_CameraController.GetCamera());
            
            float r = 5.f;
            Renderer2D::DrawCircle({ 0.f, 0.f, 0.f }, r, { 0.f, 0.f, 0.f, 1.f });

            {
                glm::vec3 prev;
                for (int i = 0; i < 361; i++)
                {
                    float k = i;
                    float r0 = r * sin(DEG2RAD(n * k));
                    float x = r0 * cos(DEG2RAD(k));
                    float y = r0 * sin(DEG2RAD(k));

                    glm::vec3 pos = { x, y, 0.f };
                    Renderer2D::DrawLine(pos, prev, { 1.f, 0.f, 1.f, 0.4f });
                    prev = pos;
                }
            }

            {
                glm::vec3 prev, start;
                for (int i = 0; i < 361; i++)
                {
                    float k = i * d;
                    float r0 = r * sin(DEG2RAD(n * k));
                    float x = r0 * cos(DEG2RAD(k));
                    float y = r0 * sin(DEG2RAD(k));

                    glm::vec3 pos = { x, y, 0.f };
                    Renderer2D::DrawLine(pos, prev, { 0.f, 0.f, 0.f, 1.f });
                    prev = pos;
                }
            }


            Renderer2D::EndScene();

            m_Framebuffer->Unbind();
        }
    }

    void EditorLayer::OnImGuiRender()
    {
        BG_PROFILE_FUNCTION();

        static bool dockspaceopen = true;

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceopen, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Windows

        ImGui::Begin("Settings");

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::Text("FPS: %f", m_FPS);

        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

        ImGui::SliderFloat("d", &d, 0, 360);
        ImGui::SliderInt("n", &n, 0, 10);

        //ImGui::Image((void*)m_CheckerboardTexture->GetRendererID(), ImVec2(256.f, 256.f));

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::Begin("Viewport");
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize))
        {
            m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
            m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);

            BG_WARN("Viewport Size: {0}, {1}", viewportPanelSize.x, viewportPanelSize.y); 
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        }

        ImGui::Image((void*)m_Framebuffer->GetColorAttachmentRendererID(), 
                    { m_ViewportSize.x, m_ViewportSize.y },
                    {0, 1}, {1, 0});

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        m_CameraController.OnEvent(e);
    }

}