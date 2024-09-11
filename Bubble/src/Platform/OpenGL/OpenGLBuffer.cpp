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
// ComputeBuffer //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

	OpenGLComputeBuffer::OpenGLComputeBuffer(uint32_t size, const void* data)
		: m_Size(size)
	{
		BG_PROFILE_FUNCTION();

		// Create the buffer and allocate memory
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);  // Set initial data (if provided)
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);  // Unbind after creation
	}

	OpenGLComputeBuffer::~OpenGLComputeBuffer()
	{
		BG_PROFILE_FUNCTION();

		// Delete the buffer when the compute buffer is destroyed
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLComputeBuffer::Bind(uint32_t bindingPoint) const
	{
		BG_PROFILE_FUNCTION();

		// Bind the buffer to the specified binding point
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_RendererID);
	}

	void OpenGLComputeBuffer::UnBind() const
	{
		BG_PROFILE_FUNCTION();

		// Unbind the buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void OpenGLComputeBuffer::SetData(const void* data, uint32_t size)
	{
		BG_PROFILE_FUNCTION();

		// Update the data in the buffer
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);  // Update the buffer's data
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);  // Unbind after updating
	}

	void* OpenGLComputeBuffer::MapBufferRange(uint32_t size, uint32_t access)
	{
		GL_MAP_READ_BIT
		BG_PROFILE_FUNCTION();

		// Map the buffer range for reading/writing
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		void* mappedData = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, access);
		return mappedData;  // Don't forget to unmap after accessing
	}

	void OpenGLComputeBuffer::UnmapBuffer()
	{
		BG_PROFILE_FUNCTION();

		// Unmap the buffer after accessing it
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	uint32_t OpenGLComputeBuffer::GetSize() const
	{
		return m_Size;  // Return the size of the buffer
	}

}
