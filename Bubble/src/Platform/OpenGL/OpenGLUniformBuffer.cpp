#include "bgpch.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

#include <glad/glad.h>

namespace Bubble {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
		: m_Size(size), m_Binding(binding)
	{
		BG_PROFILE_FUNCTION()

		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, m_Size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Binding, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		BG_PROFILE_FUNCTION()

		glDeleteBuffers(1, &m_RendererID);
	}

	uint32_t OpenGLUniformBuffer::GetSize()
	{
		return m_Size;
	}

	uint32_t OpenGLUniformBuffer::GetBinding()
	{
		return m_Binding;
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t offset)
	{
		BG_PROFILE_FUNCTION()

		glNamedBufferSubData(m_RendererID, offset, m_Size, data);
	}

}
