#include "bgpch.h"

#include "Platform/OpenGL/OpenGLTexture.h"

#include "stb_image.h"

#include <glad/glad.h>

namespace Bubble {

	namespace Utils {

		static GLenum BubbleImageFormatToGLDataFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB;
			case ImageFormat::RGBA8: return GL_RGBA;
			case ImageFormat::RGBA32F:  return GL_RGBA;
			}

			BG_CORE_ASSERT(false, "");
			return 0;
		}

		static GLenum BubbleImageFormatToGLInternalFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RGB8:  return GL_RGB8;
			case ImageFormat::RGBA8: return GL_RGBA8;
			case ImageFormat::RGBA32F:  return GL_RGBA32F;
			}

			BG_CORE_ASSERT(false, "");
			return 0;
		}

		static const char* GetGLErrorString(GLenum errorCode)
		{
			switch (errorCode)
			{
			case GL_NO_ERROR:           return "GL_NO_ERROR";
			case GL_INVALID_ENUM:       return "GL_INVALID_ENUM";
			case GL_INVALID_VALUE:      return "GL_INVALID_VALUE";
			case GL_INVALID_OPERATION:  return "GL_INVALID_OPERATION";
			case GL_STACK_OVERFLOW:     return "GL_STACK_OVERFLOW";
			case GL_STACK_UNDERFLOW:    return "GL_STACK_UNDERFLOW";
			case GL_OUT_OF_MEMORY:      return "GL_OUT_OF_MEMORY";
			case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
			default:                    return "Unknown OpenGL Error";
			}
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
	{
		BG_PROFILE_FUNCTION()

		m_InternalFormat = Utils::BubbleImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormat = Utils::BubbleImageFormatToGLDataFormat(m_Specification.Format);

		GLenum error;
#if 0
		// Immutable
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
#else
		// Mutable
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "OpenGL Error: " << error << std::endl;
		}

		// Allocate storage with glTexImage2D (mutable storage)
		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_FLOAT, nullptr);

		error = glGetError();
		if (error != GL_NO_ERROR) {
			BG_CORE_ERROR("OpenGL Error: {0}", Utils::GetGLErrorString(error));
		}

		// Set texture parameters (wrapping, filtering)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		error = glGetError();
		if (error != GL_NO_ERROR) {
			BG_CORE_ERROR("OpenGL Error: {0}", Utils::GetGLErrorString(error));
		}

		glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture
#endif

		error = glGetError();
		if (error != GL_NO_ERROR) {
			BG_CORE_ERROR("OpenGL Error: {0}", Utils::GetGLErrorString(error));
		}
		
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		: m_Path(path)
	{
		BG_PROFILE_FUNCTION()

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			BG_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}

		if (data)
		{
			m_IsLoaded = true;

			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;

			BG_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);
		}
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		BG_PROFILE_FUNCTION();

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		BG_PROFILE_FUNCTION();

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		BG_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		BG_PROFILE_FUNCTION();

		glBindTextureUnit(slot, m_RendererID);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			BG_CORE_ERROR("OpenGL Error: {0}", Utils::GetGLErrorString(error));
		}

		//glBindTexture(GL_TEXTURE_2D, m_RendererID);

		//std::vector<float> pixelData(m_Width * m_Height * 4);  // RGBA32F = 4 floats per pixel

		//// Read the texture data from OpenGL (GL_RGBA and GL_FLOAT since it's RGBA32F)
		//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixelData.data());

		//// Iterate and print the pixel values
		//std::cout << "Output Texture Data:" << std::endl;
		//for (size_t i = 0; i < pixelData.size(); i += 4) {
		//	float r = pixelData[i];
		//	float g = pixelData[i + 1];
		//	float b = pixelData[i + 2];
		//	float a = pixelData[i + 3];

		//	std::cout << "Pixel " << (i / 4) << ": R=" << r << ", G=" << g << ", B=" << b << ", A=" << a << std::endl;
		//}

		//// Unbind the texture after reading
		//glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::BindAsImage(uint32_t unit, uint32_t level, bool isWritable) const
	{
		GLenum access = isWritable ? GL_WRITE_ONLY : GL_READ_ONLY;

		glBindImageTexture(unit, m_RendererID, level, GL_FALSE, 0, access, m_InternalFormat);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			BG_CORE_ERROR("OpenGL Error: {0}", Utils::GetGLErrorString(error));
		}
	}

}

