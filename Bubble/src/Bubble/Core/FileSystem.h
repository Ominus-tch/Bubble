#pragma once

#include "Bubble/Core/Buffer.h"

namespace Bubble {

	class FileSystem
	{
	public:
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
		static bool FileChanged(const std::filesystem::path& filepath);
	};

}
