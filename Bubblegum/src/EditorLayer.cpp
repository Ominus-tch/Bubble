#include "EditorLayer.h"
#include "Bubble/Scene/SceneSerializer.h"
#include "Bubble/Utils/PlatformUtils.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../assets/scripts/CameraController.h"

#include "Bubble/Scene/SceneSerializer.h"

#include "CodingTrain.h"

namespace Bubble {

    EditorLayer::EditorLayer()
        : Layer("EditorLayer")
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

        //m_SquareEntity = m_ActiveScene->CreateEntity("Square 1");
        //m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4( 1.0f, 0.2f, 0.2f, 1.0f)).Texture = m_CheckerboardTexture;

        //m_Square2 = m_ActiveScene->CreateEntity("Square 2");
        //m_Square2.AddComponent<SpriteRendererComponent>().Texture = m_CheckerboardTexture;

        m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
        m_CameraEntity.AddComponent<CameraComponent>().Camera.SetOrthographicSize(4.f);
        //m_CameraEntity.GetComponent<TransformComponent>().Translation.x = 10.f;

        //m_SecondCamera = m_ActiveScene->CreateEntity("Second Camera");
        //m_SecondCamera.AddComponent<CameraComponent>().Primary = false;

        m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
        //m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        //SceneSerializer serializer(m_ActiveScene);
        //serializer.Serialize("assets/scenes/Example.bubble");

        //m_Y = { 100.f, 100.f, 100.f, -100.f, -100.f, -100.f, 100.f, 100.f, 100.f, -100.f, -100.f, -100.f };

        const int skip = 8;
        for (int i = 0; i < drawing->size(); i += skip)
        {
            glm::vec2& a = drawing[0][i];
            m_X.push_back(a.x);
            m_Y.push_back(a.y);
        }

        m_FourierX = dft(m_X);
        m_FourierY = dft(m_Y);
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
        m_TimeSinceLastRender = ts;
        m_Time += ts;

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

            CameraComponent comp = m_CameraEntity.GetComponent<CameraComponent>();
            TransformComponent transform = m_CameraEntity.GetComponent<TransformComponent>();

            Renderer2D::BeginScene(comp.Camera, transform.GetTransform());

            //glm::vec3 pos;

            //for (int i = 0; i < m_FourierY.size(); i++)
            //{
            //    prevPos = pos;

            //    /*int n = i * 2 + 1;
            //    float r = 75.f * (4 / (n * PI));
            //    pos += glm::vec3(
            //        r * cosf(n * m_Time),
            //        r * sinf(n * m_Time),
            //        0.f
            //    );
            //    
            //    Renderer2D::DrawCircle(prevPos, r * 2.f, {1.f, 1.f, 1.f, 0.4f});
            //    Renderer2D::DrawLine(prevPos, pos);

            //    if ((i + 1) % 30000 == 0)
            //    {
            //        Renderer2D::EndScene();
            //        Renderer2D::BeginScene(comp.Camera, transform.GetTransform());
            //    }*/
            //}

            glm::vec3 posX = { -100.f, 0.f, 0.f };
            glm::vec3 posY = { 0.f, 100.f, 0.f };

            glm::vec3 vx = EpiCycles(posX, 0, m_FourierX);
            glm::vec3 vy = EpiCycles(posY, PI / 2, m_FourierY);
            glm::vec2 v = { vx.x, vy.y };
            m_Path.insert(m_Path.begin(), v);
            Renderer2D::DrawLine({ vx.x, vx.y, 0.f }, { v.x, v.y, 0.f });
            Renderer2D::DrawLine({ vy.x, vy.y, 0.f }, { v.x, v.y, 0.f });

            glm::vec2 prevPos;
            for (int i = 0; i < m_Path.size(); i++)
            {
                glm::vec2 pos = m_Path[i];
                
                if (i != 0)
                {
                    Renderer2D::DrawLine({ prevPos.x, prevPos.y, 0.f }, { pos.x, pos.y, 0.f });
                }
                prevPos = pos;
            }

            if (m_Time > PI * 2.f)
            {
                m_Time = 0.f;
                m_Path.clear();
            }


            Renderer2D::EndScene();

            m_Framebuffer->Unbind();
        }

        m_FrameCount++;
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
                if (ImGui::MenuItem("Save Project"))
                    SaveProject();

                if (ImGui::MenuItem("Open Project...", "Ctrl+O"))
                    OpenProject();

                ImGui::Separator();

                if (ImGui::MenuItem("Open Scene"))
                    OpenScene();

                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                    SaveScene();

                if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                ImGui::Separator();

                if (ImGui::MenuItem("Exit")) Application::Get().Close();
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Panels

        m_SceneHierarchyPanel.OnImGuiRender();

        // Windows

        ImGui::Begin("Settings");

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::Separator();

        ImGui::Text("Time Since Last Render: %fms", m_TimeSinceLastRender * 1000.f);
        ImGui::Text("Time Elasped: %fs", m_Time);
        ImGui::Text("FPS: %f", m_FPS);

        ImGui::Checkbox("Animate", &m_Animate);

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::Begin("Viewport");
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        ImGui::Image((void*)m_Framebuffer->GetColorAttachmentRendererID(),
            { m_ViewportSize.x, m_ViewportSize.y },
            { 0, 1 }, { 1, 0 });

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::End();
    }

    void EditorLayer::OnEvent(Event& e)
    {
        //m_CameraController.OnEvent(e);
        //if (m_SceneState == SceneState::Edit)
        //{
        //    m_EditorCamera.OnEvent(e);
        //}

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(BG_BIND_EVENT_FN(EditorLayer::OnMouseScroll));
        //dispatcher.Dispatch<KeyPressedEvent>(BG_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BG_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnMouseScroll(MouseScrolledEvent& e)
    {
        float y = e.GetYOffset();

        auto& cam = m_CameraEntity.GetComponent<CameraComponent>().Camera;
        cam.SetOrthographicSize(cam.GetOrthographicSize() - y);

        return false;
    }

    glm::vec3 EditorLayer::EpiCycles(glm::vec3 pos, float rotation, std::vector<fourier> Fourier)
    {
        for (int i = 0; i < Fourier.size(); i++)
        {
            glm::vec3 prevPos = pos;
            fourier f = Fourier[i];
            pos += glm::vec3(
                f.amp * cosf(f.freq * m_Time + f.phase + rotation),
                f.amp * sinf(f.freq * m_Time + f.phase + rotation),
                0.f
            );

            Renderer2D::DrawCircle(prevPos, f.amp * 2.f);
            Renderer2D::DrawLine(prevPos, pos);
        }
        return pos;
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        

        return false;
    }

    //bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    //{
    //    // Shortcuts
    //    if (e.IsRepeat())
    //        return false;

    //    bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
    //    bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

    //    switch (e.GetKeyCode())
    //    {
    //    case Key::N:
    //    {
    //        if (control)
    //            NewScene();

    //        break;
    //    }
    //    case Key::O:
    //    {
    //        if (control)
    //            OpenProject();

    //        break;
    //    }
    //    case Key::S:
    //    {
    //        if (control)
    //        {
    //            if (shift)
    //                SaveSceneAs();
    //            else
    //                SaveScene();
    //        }

    //        break;
    //    }

    //    // Scene Commands
    //    case Key::D:
    //    {
    //        if (control)
    //            OnDuplicateEntity();

    //        break;
    //    }

    //    // Gizmos
    //    case Key::Q:
    //    {
    //        if (!ImGuizmo::IsUsing())
    //            m_GizmoType = -1;
    //        break;
    //    }
    //    case Key::W:
    //    {
    //        if (!ImGuizmo::IsUsing())
    //            m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
    //        break;
    //    }
    //    case Key::E:
    //    {
    //        if (!ImGuizmo::IsUsing())
    //            m_GizmoType = ImGuizmo::OPERATION::ROTATE;
    //        break;
    //    }
    //    case Key::R:
    //    {
    //        if (control)
    //        {
    //            ScriptEngine::ReloadAssembly();
    //        }
    //        else
    //        {
    //            if (!ImGuizmo::IsUsing())
    //                m_GizmoType = ImGuizmo::OPERATION::SCALE;
    //        }
    //        break;
    //    }
    //    case Key::Delete:
    //    {
    //        if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
    //        {
    //            Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
    //            if (selectedEntity)
    //            {
    //                m_SceneHierarchyPanel.SetSelectedEntity({});
    //                m_ActiveScene->DestroyEntity(selectedEntity);
    //            }
    //        }
    //        break;
    //    }
    //    }

    //    return false;
    //}

    void EditorLayer::NewProject()
    {
        //Project::New();
    }

    void EditorLayer::OpenProject(const std::filesystem::path& path)
    {
        /*if (Project::Load(path))
        {
            ScriptEngine::Init();

            OpenScene(startScenePath);
            auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
            m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();

        }*/
    }

    bool EditorLayer::OpenProject()
    {
        std::string filepath = FileDialogs::OpenFile("Bubble Project (*.bproj)\0*.bproj\0");
        if (filepath.empty())
            return false;

        OpenProject(filepath);
        return true;
    }

    void EditorLayer::SaveProject()
    {
        // Project::SaveActive();
    }

    void EditorLayer::NewScene()
    {
        m_ActiveScene = CreateRef<Scene>();
        m_SceneHierarchyPanel.SetContext(m_ActiveScene);

        m_EditorScenePath = std::filesystem::path();
    }

    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("Bubble Scene (*.bubble)\0*.bubble\0");
        if (!filepath.empty())
            OpenScene(filepath);
    }

    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        //if (m_SceneState != SceneState::Edit)
        //    OnSceneStop();

        if (path.extension().string() != ".bubble")
        {
            BG_WARN("Could not load {0} - not a scene file", path.filename().string());
            return;
        }

        Ref<Scene> newScene = CreateRef<Scene>();
        SceneSerializer serializer(newScene);
        if (serializer.Deserialize(path.string()))
        {
            m_EditorScene = newScene;
            m_SceneHierarchyPanel.SetContext(m_EditorScene);

            m_ActiveScene = m_EditorScene;
            m_EditorScenePath = path;
        }
    }

    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())
            SerializeScene(m_ActiveScene, m_EditorScenePath);
        else
            SaveSceneAs();
    }

    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("Bubble Scene (*.bubble)\0*.bubble\0");
        if (!filepath.empty())
        {
            SerializeScene(m_ActiveScene, filepath);
            m_EditorScenePath = filepath;
        }
    }

    void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(scene);
        serializer.Serialize(path.string());
    }

}