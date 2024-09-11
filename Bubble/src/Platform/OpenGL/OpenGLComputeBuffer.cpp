#include "bgpch.h"

#include "OpenGLComputeBuffer.h"

#include <glad/glad.h>

namespace Bubble {

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
