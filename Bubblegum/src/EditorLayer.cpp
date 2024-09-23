#include "EditorLayer.h"
#include "Bubble/Scene/SceneSerializer.h"
#include "Bubble/Utils/PlatformUtils.h"
#include "Bubble/Scripting/ScriptEngine.h"

#include "Bubble/Renderer/UniformBuffer.h"

#include <imgui/imgui.h>
#include "ImGuizmo.h"

#include "../assets/scripts/CameraController.h"

#include "Bubble/Scene/SceneSerializer.h"

#include <glm/gtc/type_ptr.hpp>

#include "Bubble/Core/FileSystem.h"

// Temp
#include <GLFW/include/GLFW/glfw3.h>

#define DEG2RAD PI / 180.f

namespace Bubble {

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

		//Project::New();
		//ScriptEngine::Init();
		//m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		//m_ContentBrowserPanel = CreateScope<ContentBrowserPanel>();

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto projectFilePath = commandLineArgs[1];
			OpenProject(projectFilePath);
		}
		else
		{
			// TEMP
			OpenProject("D:/Dev/Bubble/BubbleGum/SandboxProject/Sandbox.bproj");
			// TODO: prompt the user to select a directory
			// NewProject();

			// If no project is opened, close Bubblegum
			// NOTE: this is while we don't have a new project path
			//if (!OpenProject())
			//	Application::Get().Close();
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);
		m_EditorCamera.SetDistance(10.f);

		//m_CameraEntity = m_ActiveScene->CreateEntity("Main Camera");
		//auto& cc = m_CameraEntity.AddComponent<CameraComponent>();
		//cc.Camera.SetProjectionType(SceneCamera::ProjectionType::Perspective);

		//cc.Camera.SetPerspectiveNearClip(1.f);
		//cc.Camera.SetPerspectiveFarClip(10.f);

		// RayTracing
		//m_Shader = Shader::Create("SandboxProject/Assets/Shaders/RayTracing.glsl");
		////CameraUniformBuffer = UniformBuffer::Create(sizeof(CameraData), 1);

		//m_VertexArray = VertexArray::Create();

		//m_VertexBuffer = VertexBuffer::Create(sizeof(QuadVert) * 6);
		//m_VertexBuffer->SetLayout({
		//	{ ShaderDataType::Float3, "a_Position" },  // 3D position
		//	{ ShaderDataType::Float2, "a_UV" }        // UV coordinates
		//});
		//m_VertexBuffer->SetData(s_QuadVertices, sizeof(QuadVert) * 6);
		//m_VertexArray->AddVertexBuffer(m_VertexBuffer);
		// 
		//m_CameraBuffer = UniformBuffer::Create(sizeof(CameraData), 0);
		//m_EnvironmentBuffer = UniformBuffer::Create(sizeof(EnvironmentSettings), 1);
		//m_WorldDataBuffer = UniformBuffer::Create(sizeof(WorldData), 2);

		//RayTracingMaterial redMat = {
		//	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),  // colour (red)
		//	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),  // emissionColour (no emission)
		//	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),  // specularColour (white specular reflection)
		//	0.0f,                               // emissionStrength (no emission)
		//	0.5f,                               // smoothness (moderate smoothness)
		//	0.3f,                               // specularProbability (low probability of specular reflection)
		//	0                                   // flag (no special material flag)
		//};

		//RayTracingMaterial greenMat = {
		//	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),  // colour (green)
		//	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),  // emissionColour (no emission)
		//	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),  // specularColour (white specular reflection)
		//	0.0f,                               // emissionStrength (no emission)
		//	0.5f,                               // smoothness (moderate smoothness)
		//	0.3f,                               // specularProbability (low probability of specular reflection)
		//	0                                   // flag (no special material flag)
		//};

		//RayTracingMaterial blueMat = {
		//	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),  // colour (blue)
		//	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),  // emissionColour (no emission)
		//	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),  // specularColour (white specular reflection)
		//	1.0f,                               // emissionStrength (no emission)
		//	0.5f,                               // smoothness (moderate smoothness)
		//	0.3f,                               // specularProbability (low probability of specular reflection)
		//	0                                   // flag (no special material flag)
		//};

		//Sphere sphere1 = {
		//	glm::vec3(0.0f, 0.0f, 5.0f),  // position (centered, 5 units in front of the camera)
		//	1.0f,                          // radius (1 unit)
		//	redMat                         // material (red)
		//};

		//// Sphere 2: Green sphere
		//Sphere sphere2 = {
		//	glm::vec3(2.0f, 0.0f, 6.0f),  // position (slightly to the right and further away)
		//	1.5f,                          // radius (1.5 units)
		//	greenMat                       // material (green)
		//};

		//// Sphere 3: Blue sphere
		//Sphere sphere3 = {
		//	glm::vec3(-2.0f, 0.0f, 4.0f), // position (slightly to the left and closer)
		//	0.8f,                          // radius (0.8 units)
		//	blueMat                        // material (blue)
		//};

		//m_Spheres.push_back(sphere1);
		//m_Spheres.push_back(sphere2);
		//m_Spheres.push_back(sphere3);

		//m_SphereBuffer = ComputeBuffer::Create(m_Spheres.size(), sizeof(Sphere));
		// End RayTracing

		// General Shader Tests
		m_Shader = Shader::Create("SandboxProject/Assets/Shaders/Shader.glsl");

		m_ShaderDataBuffer = UniformBuffer::Create(sizeof(ShaderData), 0);

		m_VertexArray = VertexArray::Create();

		m_VertexBuffer = VertexBuffer::Create(sizeof(QuadVert) * 6);
		m_VertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_UV" },
			{ ShaderDataType::Int,    "a_EntityID" }
		});
		m_VertexBuffer->SetData(s_QuadVertices, sizeof(QuadVert) * 6);
		m_VertexArray->AddVertexBuffer(m_VertexBuffer);

		Test = Texture2D::Create("SandboxProject/Assets/Textures/street.png");

		auto test = m_ActiveScene->CreateEntity("Test");
	}

    void EditorLayer::OnDetach()
    {
        BG_PROFILE_FUNCTION();
    }

    void EditorLayer::OnUpdate(Timestep ts)
    {
        BG_PROFILE_FUNCTION();

		uint32_t width = (uint32_t)m_ViewportSize.x;
		uint32_t height = (uint32_t)m_ViewportSize.y;

		//uint32_t width = 1280;
		//uint32_t height = 720;

        if (FramebufferSpecification spec = m_Framebuffer->GetSpecification();
			width > 0 && height > 0 &&
            (spec.Width != width || spec.Height != height))
        {
            m_Framebuffer->Resize(width, height);
            m_ActiveScene->OnViewportResize(width, height);
			m_EditorCamera.SetViewportSize(width, height);

			m_OutputTexture = Texture2D::Create({ width, height, ImageFormat::RGBA32F });
			m_DestinationTexture = ComputeBuffer::Create(width * height, sizeof(glm::vec4));

			//BG_INFO(width);
			//BG_INFO(height);
			//m_OutputTexture->(m_ViewportSize.x, m_ViewportSize.y);
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			m_MouseLastClicked = { mouseY, mouseX };
		}

		if (m_Animate)
		{
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

			m_FrameCount++;
		}


        Renderer2D::ResetStats();

        m_Framebuffer->Bind();

		RenderCommand::SetClearColor({ 0.f, 0.f, 0.f, 1.f });
		//RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.f });
		RenderCommand::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		// RayTracing
		//float nearClip = m_EditorCamera.GetNearClip();
		//float planeHeight = tanf(m_EditorCamera.GetFOV() * 0.5f * DEG2RAD) * 2;
		//float planeWidth = planeHeight * m_EditorCamera.GetAspectRatio();

		//m_CameraData = {
		//	glm::vec4(m_EditorCamera.GetPosition(), 0.f),
		//	glm::vec4(planeWidth, planeHeight, 1.f, 0.f),
		//	m_EditorCamera.GetCameraToWorldMatrix()
		//};
		//m_CameraBuffer->SetData(&m_CameraData);
		//
		//m_EnvironmentBuffer->SetData(&m_EnvironmentSettings);

		//m_WorldData.PixelCount = glm::ivec2(width, height);
		//m_WorldData.Frame = m_FrameCount;
		//m_WorldData.NumSpheres = m_Spheres.size();
		//m_WorldDataBuffer->SetData(&m_WorldData);

		//m_SphereBuffer->SetData(m_Spheres.data());
		//m_SphereBuffer->Bind(0);

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y && ImGui::IsMouseDown(ImGuiMouseButton_Left))
			m_ShaderData.Mouse = glm::vec4(mouseX, mouseY, m_MouseLastClicked.x, m_MouseLastClicked.y);
		m_ShaderData.Resolution = glm::vec4(width, height, 0.f, 0.f);
		m_ShaderData.Time = m_Time;
		m_ShaderData.Frame = m_FrameCount;

		m_ShaderDataBuffer->SetData(&m_ShaderData);


		m_Shader->Bind();
		//m_Shader->SetTexture(1, "iTexture", Test);

		//Test->Bind(0);
		m_VertexArray->Bind();
		
		// TEMP: Draw function
		m_Shader->TestFunction();

		m_VertexArray->UnBind();
		m_Shader->Unbind();

		// End RayTracing

		//std::vector<glm::vec4> data(width * height);
		//m_DestinationTexture->GetData(data.data());
		//m_OutputTexture->SetData(data.data(), width * height * 4);
		//m_DestinationTexture->Unbind();
		//m_Framebuffer->SetColorAttachmentTexture(m_OutputTexture->GetRendererID());

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

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
		}

		OnOverlayRender();
		
        m_Framebuffer->Unbind();
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

#if 1
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

		ImGui::SeparatorText("Camera");

		const auto& p = m_EditorCamera.GetPosition();
        ImGui::Text("EditorCamera: (%f, %f, %f)", p.x, p.y, p.z);
		const auto& r = glm::degrees(m_EditorCamera.GetRotation());
        ImGui::Text("EditorCamera: (%f, %f, %f)", r.x, r.y, r.z);

		float nearClip = m_EditorCamera.GetNearClip();
		if (ImGui::DragFloat("Near Clip", &nearClip))
		{
			m_EditorCamera.SetNearClip(nearClip);
		}
		float farClip = m_EditorCamera.GetFarClip();
		if (ImGui::DragFloat("Far Clip", &farClip))
		{
			m_EditorCamera.SetFarClip(farClip);
		}

		float fov = m_EditorCamera.GetFOV();
		if (ImGui::DragFloat("FOV", &fov))
		{
			m_EditorCamera.SetFOV(fov);
		}

		ImGui::SeparatorText("Ray Tracing");

		ImGui::DragInt("Bounces", &m_WorldData.MaxBounceCount);
		ImGui::DragInt("Rays Per Pixel", &m_WorldData.NumRaysPerPixel);

		ImGui::SeparatorText("Environment");

		bool env = m_EnvironmentSettings.EnvironmentEnabled;
		if (ImGui::Checkbox("Environment", &env))
		{
			if (env)
				m_EnvironmentSettings.EnvironmentEnabled = 1;
			else
				m_EnvironmentSettings.EnvironmentEnabled = 0;
		}

		ImGui::ColorEdit4("Ground Colour", &m_EnvironmentSettings.GroundColour.x);
		ImGui::ColorEdit4("Sky Colour Horizon", &m_EnvironmentSettings.SkyColourHorizon.x);
		ImGui::ColorEdit4("Sky Colour Zenith", &m_EnvironmentSettings.SkyColourZenith.x);
		ImGui::DragFloat("Sun Focus", &m_EnvironmentSettings.SunFocus);
		ImGui::DragFloat("Sun Intensity", &m_EnvironmentSettings.SunIntensity);

		ImGui::SeparatorText("Spheres");

		for (auto& sphere : m_Spheres)
		{
			ImGui::PushID(&sphere);
			ImGui::Separator();
			ImGui::DragFloat3("Pos", &sphere.position.x, 0.01f);
			ImGui::DragFloat("Radius", &sphere.radius, 0.01f);
			DrawMaterialControls(sphere.material);
			ImGui::PopID();
		}

        ImGui::Checkbox("Animate", &m_Animate);

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
		//m_ViewportSize = { 1280, 720 };

        //ImGui::Image((void*)m_OutputTexture->GetRendererID(),
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

	void EditorLayer::DrawMaterialControls(RayTracingMaterial& mat)
	{
		ImGui::ColorEdit4("Colour", &mat.colour.x);
		ImGui::ColorEdit4("Emission Colour", &mat.emissionColour.x);
		ImGui::ColorEdit4("Specular Colour", &mat.specularColour.x);
		ImGui::DragFloat("Emission Strength", &mat.emissionStrength);
		ImGui::DragFloat("Smoothness", &mat.smoothness);
		ImGui::DragFloat("Specular Probability", &mat.specularProbability);
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
