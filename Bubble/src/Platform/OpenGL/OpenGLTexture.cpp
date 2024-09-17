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

			BG_CORE_ASSERT(false, "Unknown ImageFormat!");
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

			BG_CORE_ASSERT(false, "Unknown ImageFormat!");
			return 0;
		}

	}

	OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification& specification)
		: m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
	{
		BG_PROFILE_FUNCTION()

		m_InternalFormat = Utils::BubbleImageFormatToGLInternalFormat(m_Specification.Format);
		m_DataFormat = Utils::BubbleImageFormatToGLDataFormat(m_Specification.Format);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
		BG_PROFILE_FUNCTION()

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		BG_PROFILE_FUNCTION();

		uint32_t bpp;
		GLenum type = GL_UNSIGNED_BYTE;

		// Handle different formats, assuming m_DataFormat is GL_RGBA or GL_RGBA32F
		if (m_InternalFormat == GL_RGBA32F) {
			bpp = 4 * sizeof(float);
			type = GL_FLOAT;
		}
		else if (m_InternalFormat == GL_RGBA) {
			bpp = 4;
			type = GL_UNSIGNED_BYTE;
		}
		else {
			if (m_InternalFormat == GL_RGB32F) {
				bpp = 3 * sizeof(float);
				type = GL_FLOAT;
			}
			else if (m_InternalFormat == GL_RGB) {
				bpp = 3;
				type = GL_UNSIGNED_BYTE;
			}
		}

		BG_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, type, data);
	}

	void OpenGLTexture2D::GetData(void* data)
	{
		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		uint32_t size = m_Width * m_Height * bpp;

		glGetTextureImage(m_RendererID, 0, m_DataFormat, GL_UNSIGNED_BYTE, size, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		BG_PROFILE_FUNCTION()

		glBindTextureUnit(slot, m_RendererID);
	}

	void OpenGLTexture2D::BindAsImage(uint32_t unit, GLenum access = GL_WRITE_ONLY) const
	{
		BG_PROFILE_FUNCTION()

		glBindImageTexture(0, m_RendererID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	}

}

