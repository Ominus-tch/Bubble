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
			}

			BG_CORE_ASSERT(false, "");
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

		stbi_uc* data;
		{
			BG_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std::string&)")
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
		BG_CORE_ASSERT(data, "Failed to load image: {0}", path);
		m_Width = width;
		m_Height = height;

		GLenum internalFormat = 0, dataFormat = 0;
		switch (channels) {
		case 4: {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
			break;
		}
		case 3: {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			break;
		}
		default: {
			BG_CORE_ASSERT(0, "Format not supported: {0} in file {1}", channels, path);
		}
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		BG_PROFILE_FUNCTION()

		glDeleteTextures(1, &m_RendererID);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		BG_PROFILE_FUNCTION()

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		BG_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void OpenGLTexture2D::Bind(uint32_t slot) const
	{
		BG_PROFILE_FUNCTION()

		glBindTextureUnit(slot, m_RendererID);
	}

}

