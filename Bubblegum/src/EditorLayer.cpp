#include "EditorLayer.h"
#include "Bubble/Scene/SceneSerializer.h"
#include "Bubble/Utils/PlatformUtils.h"
#include "Bubble/Scripting/ScriptEngine.h"

#include "Bubble/Renderer/ComputeBuffer.h"
#include "Bubble/Renderer/UniformBuffer.h"

#include <imgui/imgui.h>
#include "ImGuizmo.h"

#include "../assets/scripts/CameraController.h"

#include "Bubble/Scene/SceneSerializer.h"

#include <glm/gtc/type_ptr.hpp>

// Temp
#include <GLFW/include/GLFW/glfw3.h>

namespace Bubble {

	static float height = 100.f;
	static float width = 100.f;

	static int s_Depth = 10;
	static float s_Angle = PI/4;
	static float s_LenScalar = 0.67f;

    EditorLayer::EditorLayer()
        : Layer("EditorLayer")
    {
    }

    void EditorLayer::OnAttach()
    {
        BG_PROFILE_FUNCTION();

        m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		m_IconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_IconPause = Texture2D::Create("Resources/Icons/PauseButton.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");
		m_IconStep = Texture2D::Create("Resources/Icons/StepButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/StopButton.png");

        FramebufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
        fbSpec.Width = 1280; fbSpec.Height = 720;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        //m_CameraController.SetZoomLevel(5.f);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
		}
		else
		{
			// TEMP
			//OpenProject("C:/Dev/Bubble/BubbleGum/SandboxProject/Sandbox.bproj");
			// TODO: prompt the user to select a directory
			// NewProject();

			// If no project is opened, close Bubblegum
			// NOTE: this is while we don't have a new project path
			//if (!OpenProject())
			//	Application::Get().Close();
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		//m_Shader = Shader::Create("SandboxProject/Assets/Shaders/TestShader.glsl");

		//uint32_t numElements = 1024;
		//Ref<ComputeBuffer> outputBuffer = ComputeBuffer::Create(numElements, sizeof(uint32_t));

		//// Initialize the buffer with zeroes (optional)
		//std::vector<uint32_t> initialData(numElements, 0);
		//outputBuffer->SetData(initialData.data());

		//m_Shader->SetBuffer(0, "OutputBuffer", outputBuffer);

		//m_Shader->Bind();
		//m_Shader->DispatchCompute(numElements, 1, 1);
		//m_Shader->Unbind();

		//std::vector<uint32_t> outputData(numElements);
		//outputBuffer->GetData(outputData.data());

		//for (uint32_t i = 0; i < 10; ++i) {
		//	std::cout << "outputData[" << i << "] = " << outputData[i] << std::endl;
		//}
    }

    void EditorLayer::OnDetach()
    {
        BG_PROFILE_FUNCTION();
    }

	static void branch(const glm::vec2& pos, glm::vec2 direction, float len, int depth = 0)
	{
		if (depth >= s_Depth) return;

		glm::vec2 newPos;

		//// Handle the initial case where the starting position is (0, 0)
		//if (depth == 0 && glm::length(pos) == 0.0f)
		//{
		//	direction = glm::vec2(0.0f, 1.0f); // Start direction pointing upwards (along y-axis)
		//}

		direction = glm::normalize(direction);

		// Calculate the new position after moving in the current direction
		newPos = pos + direction * len;

		// Draw the line from the current position to the new position
		Renderer2D::DrawLine(pos, newPos);

		// Rotate the direction vector by 45 degrees (PI/4 radians) clockwise and counterclockwise
		float angle = s_Angle;
		glm::mat2 rotationMatrixClockwise = glm::mat2(
			glm::cos(-angle), -glm::sin(-angle),
			glm::sin(-angle), glm::cos(-angle)
		);

		glm::mat2 rotationMatrixCounterclockwise = glm::mat2(
			glm::cos(angle), -glm::sin(angle),
			glm::sin(angle), glm::cos(angle)
		);

		// Increase depth
		depth++;

		// Recursive calls for the two branches
		branch(newPos, rotationMatrixClockwise * direction, len * s_LenScalar, depth);
		branch(newPos, rotationMatrixCounterclockwise * direction, len * s_LenScalar, depth);
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
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
        }

		float fps = 1.f / ts;

		m_FPSCache.push_front(fps);
		m_FPSSum += fps;

		if (m_FPSCache.size() > MaxCacheSize)
		{
			m_FPSSum -= m_FPSCache.back();
			m_FPSCache.pop_back();
		}

		m_FPS = m_FPSSum / m_FPSCache.size();

        m_TimeSinceLastRender = ts;
		m_Time += ts;

        Renderer2D::ResetStats();
        m_Framebuffer->Bind();
        //RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1.f });
        RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
        RenderCommand::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		if (m_Animate)
			s_Angle += ts;

		Renderer2D::BeginScene(m_EditorCamera);

		CelestialBody cb = CelestialBody({}, {}, 0.1f);
		cb.Draw();
		
		Renderer2D::EndScene();

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				//if (m_ViewportFocused)
				//	m_CameraController.OnUpdate(ts);

				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Simulate:
			{
				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		OnOverlayRender();
		
        m_Framebuffer->Unbind();
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
		if (m_ContentBrowserPanel)
			m_ContentBrowserPanel->OnImGuiRender();


        // Windows

        ImGui::Begin("Stats");

#if 0
		std::string name = "None";
		if (m_HoveredEntity)
			name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
		ImGui::Text("Hovered Entity: %s", name.c_str());
#endif

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads: %d", stats.QuadCount);
        ImGui::Text("Triangles: %d", stats.TriCount);
        ImGui::Text("Lines: %d", stats.LineCount);
        ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

        ImGui::Separator();

        //ImGui::Text("Time Since Last Render: %fms", m_TimeSinceLastRender * 1000.f);
        ImGui::Text("Time Elasped: %.2fs", m_Time);
        ImGui::Text("FPS: %i", (int)m_FPS);

        ImGui::Checkbox("Animate", &m_Animate);
		ImGui::InputInt("Depth", &s_Depth);

		ImGui::DragFloat("Angle", &s_Angle, 0.01f, -PI, PI);
		ImGui::DragFloat("Len", &s_LenScalar, 0.01f);

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
        ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

        ImGui::Image((void*)m_Framebuffer->GetColorAttachmentRendererID(),
            { m_ViewportSize.x, m_ViewportSize.y },
            { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(path);
			}
			ImGui::EndDragDropTarget();
		}

		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Runtime camera from entity
			// Entity cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const CameraComponent& camera = cameraEntity.GetComponent<CameraComponent>();
			// const glm::mat4& cameraProjection = camera.Camera.GetProjection();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();


			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

        ImGui::End();
        ImGui::PopStyleVar();

		UI_Toolbar();

        ImGui::End();
    }

	void EditorLayer::UI_Toolbar()
	{
		if (!m_ActiveScene)
			return;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_ActiveScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		bool hasPlayButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play;
		bool hasSimulateButton = m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate;
		bool hasPauseButton = m_SceneState != SceneState::Edit;

		if (hasPlayButton)
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			if (ImGui::ImageButton("PlayStopButton", (ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
		}

		if (hasSimulateButton)
		{
			if (hasPlayButton)
				ImGui::SameLine();

			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			if (ImGui::ImageButton("SimulateStopButton", (ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}
		if (hasPauseButton)
		{
			bool isPaused = m_ActiveScene->IsPaused();
			ImGui::SameLine();
			{
				Ref<Texture2D> icon = m_IconPause;
				if (ImGui::ImageButton("PauseButton", (ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
				{
					m_ActiveScene->SetPaused(!isPaused);
				}
			}

			// Step button
			if (isPaused)
			{
				ImGui::SameLine();
				{
					Ref<Texture2D> icon = m_IconStep;
					bool isPaused = m_ActiveScene->IsPaused();
					if (ImGui::ImageButton("StepButton", (ImTextureID)(uint64_t)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tintColor) && toolbarEnabled)
					{
						m_ActiveScene->Step();
					}
				}
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
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
        dispatcher.Dispatch<KeyPressedEvent>(BG_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BG_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnMouseScroll(MouseScrolledEvent& e)
    {
        /*float y = e.GetYOffset();

        auto& cam = m_CameraEntity.GetComponent<CameraComponent>().Camera;
        cam.SetOrthographicSize(cam.GetOrthographicSize() - y);*/

        return false;
    }

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera)
				return;

			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}

		// Draw selected entity outline 
		if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
		{
			const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
			Renderer2D::DrawRect(transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
		}

		Renderer2D::EndScene();
	}

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        // Shortcuts
        if (e.IsRepeat())
            return false;

        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

        switch (e.GetKeyCode())
        {
        case Key::N:
        {
            if (control)
                NewScene();

            break;
        }
        case Key::O:
        {
            if (control)
                OpenProject();

            break;
        }
        case Key::S:
        {
            if (control)
            {
                if (shift)
                    SaveSceneAs();
                else
                    SaveScene();
            }

            break;
        }

        // Scene Commands
        case Key::D:
        {
            if (control)
               OnDuplicateEntity();

            break;
        }

        // Gizmos
        case Key::Q:
        {
            if (shift && !ImGuizmo::IsUsing())
                m_GizmoType = -1;
            break;
        }
        case Key::W:
        {
            if (shift && !ImGuizmo::IsUsing())
                m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        }
        case Key::E:
        {
            if (shift && !ImGuizmo::IsUsing())
                m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        }
        case Key::R:
        {
            if (control)
            {
                ScriptEngine::ReloadAssembly();
            }
            else
            {
                if (shift && !ImGuizmo::IsUsing())
                    m_GizmoType = ImGuizmo::OPERATION::SCALE;
            }
            break;
        }
        case Key::Delete:
        {
            if (Application::Get().GetImGuiLayer()->GetActiveWidgetID() == 0)
            {
                Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
                if (selectedEntity)
                {
                    m_SceneHierarchyPanel.SetSelectedEntity({});
                    m_ActiveScene->DestroyEntity(selectedEntity);
                }
            }
            break;
        }
        }

        return false;
    }

    void EditorLayer::NewProject()
    {
        Project::New();
    }

    void EditorLayer::OpenProject(const std::filesystem::path& path)
    {
        if (Project::Load(path))
        {
            ScriptEngine::Init();

            auto startScenePath = Project::GetAssetFileSystemPath(Project::GetActive()->GetConfig().StartScene);
            OpenScene(startScenePath);
            m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();
        }
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
        Project::SaveActive("C:/Dev/Bubble/BubbleGum/SandboxProject/Sandbox.bproj");
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
        if (m_SceneState != SceneState::Edit)
            OnSceneStop();

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

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		BG_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnScenePause()
	{
		if (m_SceneState == SceneState::Edit)
			return;

		m_ActiveScene->SetPaused(true);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
		{
			Entity newEntity = m_EditorScene->DuplicateEntity(selectedEntity);
			m_SceneHierarchyPanel.SetSelectedEntity(newEntity);
		}
	}

}
