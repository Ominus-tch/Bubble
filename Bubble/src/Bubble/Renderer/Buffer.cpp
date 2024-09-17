#include "bgpch.h"

#include "Bubble/Renderer/Buffer.h"
#include "Bubble/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Bubble {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    BG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size);
		}

		BG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	BG_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		BG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	BG_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:	return CreateRef<OpenGLIndexBuffer>(indices, count);
		}

		BG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

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
