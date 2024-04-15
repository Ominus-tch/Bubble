#include "bgpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Bubble {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}