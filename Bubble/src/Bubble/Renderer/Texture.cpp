#include "bgpch.h"

#include "Bubble/Renderer/Renderer.h"
#include "Bubble/Renderer/Texture.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Bubble {

	std::unordered_map<std::string, Ref<Texture>> TextureLibrary::m_Textures;

	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
	{
		Ref<Texture2D> tex = nullptr;

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    BG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  tex = CreateRef<OpenGLTexture2D>(specification); break;
		default: BG_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}

		return tex;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		if (TextureLibrary::Has(path))
			return TextureLibrary::Get2D(path);

		Ref<Texture2D> tex;

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    BG_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  tex = CreateRef<OpenGLTexture2D>(path); break;
		default:                        BG_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}

		TextureLibrary::Add(path, tex);
		return tex;
	}
}

