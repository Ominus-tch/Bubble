#include "bgpch.h"

#include "Bubble/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Bubble {

	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}