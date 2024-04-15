#include "bgpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
//#include "Platform/Direct3D/Direct3DBuffer.h"

namespace Bubble {



	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		BG_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return new OpenGLVertexBuffer(vertices, size);
		}

		BG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		BG_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return new OpenGLIndexBuffer(indices, count);
		}

		BG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}