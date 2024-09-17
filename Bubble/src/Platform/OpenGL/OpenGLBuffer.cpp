#include "bgpch.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

#include <glad/glad.h>

namespace Bubble {

	//////////////////////////////////////////////////////////////////
	// VertexBuffer //////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		BG_PROFILE_FUNCTION()

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size)
	{
		BG_PROFILE_FUNCTION()

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		BG_PROFILE_FUNCTION()

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		BG_PROFILE_FUNCTION()

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::UnBind() const
	{
		BG_PROFILE_FUNCTION()

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		BG_PROFILE_FUNCTION()

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	//////////////////////////////////////////////////////////////////
	// IndexBuffer ///////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{
		BG_PROFILE_FUNCTION()

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		BG_PROFILE_FUNCTION()

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		BG_PROFILE_FUNCTION()

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		BG_PROFILE_FUNCTION()

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	//////////////////////////////////////////////////////////////////
	// ComputeBuffer /////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	OpenGLComputeBuffer::OpenGLComputeBuffer(uint32_t count, uint32_t stride)
		: m_Count(count), m_Stride(stride)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, GetBufferSize(), nullptr, GL_DYNAMIC_DRAW); // Allocate space
	}

	OpenGLComputeBuffer::~OpenGLComputeBuffer()
	{
		Release();
	}

	void OpenGLComputeBuffer::Bind(uint32_t slot) const
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_RendererID);
	}

	void OpenGLComputeBuffer::Unbind() const
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void OpenGLComputeBuffer::SetData(const void* data)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, GetBufferSize(), data);  // Update the data
	}

	void OpenGLComputeBuffer::GetData(void* data)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		void* mappedData = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, GetBufferSize(), GL_MAP_READ_BIT);
		if (mappedData)
		{
			memcpy(data, mappedData, GetBufferSize());
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		}
	}

	void OpenGLComputeBuffer::Release()
	{
		if (m_RendererID)
		{
			glDeleteBuffers(1, &m_RendererID);
			m_RendererID = 0;  // Set to 0 to indicate that the buffer has been released
		}
	}

	uint32_t OpenGLComputeBuffer::GetRendererID() const
	{
		return m_RendererID;
	}

}
