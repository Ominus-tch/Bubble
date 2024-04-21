#include "bgpch.h"

#include "Bubble/Renderer/Renderer.h"
#include "Bubble/Renderer/UniformBuffer.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Bubble {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    BG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		BG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}