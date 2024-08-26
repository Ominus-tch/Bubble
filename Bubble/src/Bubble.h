#pragma once

// Client applications

// ----Core-------------------------
#include "Bubble/Core/Application.h"
#include "Bubble/Core/Layer.h"
#include "Bubble/Core/Log.h"

#include "Bubble/Core/Input.h"
#include "Bubble/Core/KeyCodes.h"
#include "Bubble/Core/MouseCodes.h"

#include "Bubble/Core/Timestep.h"
#include "Bubble/Core/Random.h"
// ---------------------------------

// ----Math-------------------------
#include "Bubble/Math/Math.h"
// ---------------------------------

// ----Utils------------------------
#include "Bubble/Utils/PlatformUtils.h"
// ---------------------------------

// ----ImGui------------------------
#include "Bubble/ImGui/ImGuiLayer.h"
// ---------------------------------

// ----Project----------------------
#include "Bubble/Project/Project.h"
// ---------------------------------

// ----Renderer---------------------
#include "Bubble/Renderer/Renderer.h"
#include "Bubble/Renderer/Renderer2D.h"
#include "Bubble/Renderer/RenderCommand.h"

#include "Bubble/Renderer/Buffer.h"
#include "Bubble/Renderer/Shader.h"
#include "Bubble/Renderer/Framebuffer.h"
#include "Bubble/Renderer/Texture.h"
#include "Bubble/Renderer/SubTexture2D.h"
#include "Bubble/Renderer/Mesh.h"
#include "Bubble/Renderer/VertexArray.h"

#include "Bubble/Renderer/OrthographicCamera.h"
#include "Bubble/Renderer/OrthographicCameraController.h"
// ---------------------------------

// ----Scene------------------------
#include "Bubble/Scene/Scene.h"
#include "Bubble/Scene/Entity.h"
#include "Bubble/Scene/ScriptableEntity.h"
#include "Bubble/Scene/Components.h"
// ---------------------------------
