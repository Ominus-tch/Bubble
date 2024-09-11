#include "bgpch.h"

#include "Bubble/Renderer/Renderer.h"
#include "Bubble/Renderer/ComputeBuffer.h"

#include "Platform/OpenGL/OpenGLComputeBuffer.h"

namespace Bubble {

	Ref<ComputeBuffer> ComputeBuffer::Create(uint32_t count, uint32_t stride)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None: BG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL: return CreateRef<OpenGLComputeBuffer>(count, stride);
		}

		BG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
