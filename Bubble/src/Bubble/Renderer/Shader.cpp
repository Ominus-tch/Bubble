#include "bgpch.h"

#include "Bubble/Renderer/Renderer.h"
#include "Bubble/Renderer/Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Bubble {

	std::unordered_map<std::string, Ref<Shader>> ShaderLibrary::m_Shaders;

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		if (ShaderLibrary::Has(name))
			return ShaderLibrary::Get(name);

		Ref<Shader> shader = nullptr;

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	BG_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	shader = CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc, ""); break;
		default: BG_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}

		ShaderLibrary::Add(name, shader);
		return shader;
	}

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		if (ShaderLibrary::Has(filepath))
			return ShaderLibrary::Get(filepath);

		Ref<Shader> shader = nullptr;

		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	BG_CORE_ASSERT(false, "RendererAPI::None is not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:	shader = CreateRef<OpenGLShader>(filepath); break;
		default: BG_CORE_ASSERT(false, "Unknown RendererAPI!"); return nullptr;
		}

		ShaderLibrary::Add(filepath, shader);
		return shader;
	}

	void ShaderLibrary::Add(const std::string& filepath, const Ref<Shader>& shader)
	{
		if (!Has(filepath))
		m_Shaders[filepath] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& filepath = shader->GetPath();
		if (!filepath.empty())
			Add(filepath, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	bool ShaderLibrary::Has(const std::string& filepath)
	{
		return m_Shaders.find(filepath) != m_Shaders.end();
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& filepath)
	{
		if (Has(filepath))
			return m_Shaders[filepath];
		return nullptr;
	}

	std::unordered_map<std::string, Ref<Shader>> ShaderLibrary::GetShaders()
	{
		return m_Shaders;
	}

}
