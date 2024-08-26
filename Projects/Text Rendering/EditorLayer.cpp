#include "EditorLayer.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

        //m_Boundries.push_back({ {300, 100}, {300, 300} });

        m_CameraController.SetZoomLevel(500.f);
        FontReader reader = FontReader("assets/fonts/JetBrainsMono-Bold.ttf");

        reader.Skip(4);
        uint16_t numTables = reader.Read<uint16_t>();
        reader.Skip(6);
        BG_INFO(numTables);

        for (int i = 0; i < numTables; i++)
        {
            std::string tag = reader.ReadTag();
            uint32_t checksum = reader.Read<uint32_t>();
            uint32_t offset = reader.Read<uint32_t>();
            uint32_t length = reader.Read<uint32_t>();
            reader.TableInfo[tag] = { checksum, offset, length };
        }

        reader.Goto(reader.TableInfo["glyf"].offset);
        glyph0 = reader.ReadSimpleGlyph();
        BG_INFO(glyph0.ToString());
    }

    void EditorLayer::OnDetach()
    {
        BG_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        BG_PROFILE_FUNCTION();


        //if (m_ViewportFocused)
        //{
        //    glm::vec2 mousePos = Input::GetMousePosition() - m_WindowPos;
        //    glm::vec2 screenDims = { m_Framebuffer->GetSpecification().Width, m_Framebuffer->GetSpecification().Height };
        //    glm::vec2 cameraPos = m_CameraController.GetCameraPos();
        //    float zoomLevel = m_CameraController.GetZoomLevel();

        //    //BG_INFO("Mouse Pos: {0}, {1}", mousePos.x, mousePos.y);
        //    //BG_INFO("Screen Dims: {0}, {1}", screenDims.x, screenDims.y);
        //    //BG_INFO("Cam Pos: {0}, {1}", cameraPos.x, cameraPos.y);
        //    //BG_INFO("Zoom Level: {0}", zoomLevel);

        //    //Mouse Pos: 844, 174
        //    //Screen Dims: 1280, 682
        //    //Cam Pos: 0, 0
        //    //Zoom Level: 500

        //    //Expected Result 300, 300.

        //    //Result: 0.408, 0.334

        //    m_MouseWorldPos = Utils::ScreenToWorld(mousePos, screenDims, cameraPos, zoomLevel);

        //    //BG_INFO("{0}, {1}", m_MouseWorldPos.x, m_MouseWorldPos.y);

        //    /*if (m_FrameCount < 10)
        //    {
        //        m_FrameCount++;
        //        Random::Next(ts);
        //    }*/

        //    //m_LoopTime += ts;
    
        //}

        m_FPS = 1.f / ts;

        // Update
        if (m_ViewportFocused)
            m_CameraController.OnUpdate(ts);
    }

    void EditorLayer::OnRender(Timestep ts)
    {
        Renderer2D::ResetStats();
        {
            BG_PROFILE_SCOPE("Renderer Prep");
            m_Framebuffer->Bind();
            //RenderCommand::SetClearColor({ 1.f, 1.f, 1.f, 1 });
            RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
            RenderCommand::Clear();
        }

        {
            BG_PROFILE_SCOPE("Renderer Draw");
            Renderer2D::BeginScene(m_CameraController.GetCamera());

            DrawGlyph(glyph0);
            
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

        //ImGui::Image((void*)m_CheckerboardTexture->GetRendererID(), ImVec2(256.f, 256.f));

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();

        ImGuiLayer* imGuiLayer = Application::Get().GetImGuiLayer();
        imGuiLayer->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

        /*ImGuiContext& g = *GImGui;
        auto* focused = g.NavWindow;

        if (ImGui::GetCurrentWindow() == focused)
        {
            BG_INFO("Focused");
        }*/

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        ImVec2 windowPos = ImGui::GetWindowPos();

        m_WindowPos = *((glm::vec2*)&windowPos);
        //BG_INFO("Window: {0}, {1}", m_WindowPos.x, m_WindowPos.y);

        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
        {
            m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
            m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);

            //BG_WARN("Viewport Size: {0}, {1}", viewportPanelSize.x, viewportPanelSize.y); 
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
        //EventDispatcher dispatcher(e);
        //dispatcher.Dispatch<WindowCloseEvent>(BG_BIND_EVENT_FN(Application::OnWindowClose));


        m_CameraController.OnEvent(e);
    }

}