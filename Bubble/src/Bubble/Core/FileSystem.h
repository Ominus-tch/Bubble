#pragma once

#include "Bubble/Core/Buffer.h"

namespace Bubble {

	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
		static bool FileChanged(const std::filesystem::path& filepath);
		static std::vector<std::string> ScanAssetsDirectory(bool initScan = false, const std::filesystem::path& projectDirectory = "SandboxProject", const std::filesystem::path& assetsDirectory = "assets");
	private:
		static std::vector<std::string> m_ChangedSinceLastRun;
		static std::filesystem::path s_CachePath;
	};

}
