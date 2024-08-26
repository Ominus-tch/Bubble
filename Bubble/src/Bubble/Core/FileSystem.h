#pragma once

#include "Bubble/Core/Buffer.h"

namespace Bubble {

	class FileSystem
	{
	public:
		// TODO: move to FileSystem class
		static Buffer ReadFileBinary(const std::filesystem::path& filepath);
	};

}
