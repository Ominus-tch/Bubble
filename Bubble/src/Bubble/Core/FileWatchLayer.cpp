#include "bgpch.h"
#include "Bubble/Core/FileWatchLayer.h"

#include "Bubble/Renderer/Shader.h"
#include "Bubble/Renderer/Material.h"
#include "Bubble/Renderer/Texture.h"

namespace Bubble {

	FileWatchLayer::FileWatchLayer()
		: Layer("FileWatchLayer")
	{

	}

	FileWatchLayer::~FileWatchLayer()
	{

	}

	void FileWatchLayer::OnAttach()
	{
		BG_PROFILE_FUNCTION();

		FileSystem::ScanAssetsDirectory(true);
	}

	void FileWatchLayer::OnDetach()
	{
		BG_PROFILE_FUNCTION();

	}

	static std::string NormalizePath(std::string path) {
		std::replace(path.begin(), path.end(), '\\', '/');
		return path;
	}

	void FileWatchLayer::OnUpdate(Timestep ts)
	{
		m_TimeSinceLastScan += ts;

		if (m_TimeSinceLastScan >= m_ScanInverval)
		{
			m_TimeSinceLastScan = 0.f;

			auto& changed = FileSystem::ScanAssetsDirectory();

			std::unordered_map<std::string, Ref<Shader>>& allShaders = ShaderLibrary::GetShaders();

			for (auto& p : changed)
			{
				p = NormalizePath(p);
			}

			for (auto& p : changed)
			{
				BG_INFO(p);
			}

			for (const auto& pair : allShaders)
			{
				const auto& path = NormalizePath(pair.first);
				if (changed.size() > 0)
					BG_INFO("2: {0}", path);

				if (std::find(changed.begin(), changed.end(), path) == changed.end())
					continue;


				Ref<Shader> shader = pair.second;
				if (shader->Recompile())
				{
					BG_CORE_INFO("Shader {0} recompiled sucessesfully", shader->GetPath());
				}
				else
				{
					BG_CORE_INFO("Shader {0} recompile fail", shader->GetPath());
				}
			}
		}
	}
}
