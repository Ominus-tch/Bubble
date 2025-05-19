#pragma once

#include "Bubble/Core/Log.h"
#include "Bubble/Renderer/Material.h"

namespace Bubble
{
	class MaterialSerializer
	{
	public:
		static Ref<Material> DeserializeMaterial(const std::string& path);
		static bool SerializeMaterial(Ref<Material> mat, const std::string& path);
	private:
	};
}
