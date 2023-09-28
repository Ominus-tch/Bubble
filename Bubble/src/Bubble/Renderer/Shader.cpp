#include "bgpch.h"
#include "Shader.h"

#include <glad/glad.h>

namespace Bubble {

	Shader::Shader(const std::string& vertexSrc, const std::string& fragSrc)
	{
		const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(vertexShader);

			BG_CORE_ERROR("{0}", infoLog.data());
			BG_CORE_ASSERT(false, "Vertex shader compilation failure!");

			return;
		}

		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = fragSrc.c_str();
		glShaderSource(fragShader, 1, &source, 0);
		glCompileShader(fragShader);

		isCompiled = 0;
		glGetShaderiv(fragShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(fragShader);

			BG_CORE_ERROR("{0}", infoLog.data());
			BG_CORE_ASSERT(false, "Fragment shader compilation failure!");

			return;
		}

		m_RendererID = glCreateProgram();

		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragShader);

		glLinkProgram(m_RendererID);

		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, &infoLog[0]);


			glDeleteProgram(m_RendererID);

			glDeleteShader(vertexShader);
			glDeleteShader(fragShader);

			BG_CORE_ERROR("{0}", infoLog.data());
			BG_CORE_ASSERT(false, "Program linking failure!");
		}

		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragShader);

		BG_CORE_INFO("Succesfully created Shader");
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

}