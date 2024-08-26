#include "EditorLayer.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CameraController.h"

namespace Bubble {

    EditorLayer::EditorLayer()
        : Layer("EditorLayer"), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
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

        //m_CameraController.SetZoomLevel(5.f);

        m_ActiveScene = CreateRef<Scene>();

        m_SquareEntity = m_ActiveScene->CreateEntity("Square");
        m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4(0.f, 1.f, 0.f, 1.f));

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
        m_CameraEntity.AddComponent<CameraComponent>();

        m_SecondCamera = m_ActiveScene->CreateEntity("Clip-Space Entity");
        auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
        cc.Primary = false;

        //m_SecondCamera.AddComponent<NativeScriptComponent>();

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
    }

    void EditorLayer::OnDetach()
    {
        BG_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        BG_PROFILE_FUNCTION();

        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification(); 
            m_ViewportSize.x > 0.f && m_ViewportSize.y > 0.f &&
            (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        }

        m_FPS = 1.f / ts;

        m_SquareEntity.GetComponent<SpriteRendererComponent>().Color = m_SquareColor;

        Renderer2D::ResetStats();
        {
            BG_PROFILE_SCOPE("Renderer Prep");
            m_Framebuffer->Bind();
            //RenderCommand::SetClearColor({ 1.f, 1.f, 1.f, 1.f });
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
            RenderCommand::Clear();
        }

        {
            BG_PROFILE_SCOPE("Renderer Draw");

            m_ActiveScene->OnUpdateRuntime(ts);

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
        ImGui::Separator();

        ImGui::DragFloat3("Camera Transform",
            glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Translation));

        if (ImGui::Checkbox("Camera A", &m_PrimaryCamera))
        {
            m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
            m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
        }

        auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
        float orthoSize = camera.GetOrthographicSize();
        if (ImGui::DragFloat("Orthographic", &orthoSize))
        {
            camera.SetOrthographicSize(orthoSize);
        }

        //ImGui::Image((void*)m_CheckerboardTexture->GetRendererID(), ImVec2(256.f, 256.f));

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::Begin("Viewport");
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        ImGui::Image((void*)m_Framebuffer->GetColorAttachmentRendererID(), 
                    { m_ViewportSize.x, m_ViewportSize.y },
                    {0, 1}, {1, 0});

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& e)
    {

    }

}