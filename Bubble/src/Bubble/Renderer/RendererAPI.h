#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Bubble {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0, OpenGL = 1, Direct3D = 2
		};
	public:
		virtual void SetClearColor(const glm::vec4& col) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

		static inline API GetAPI() { return s_API; }
	private:
		static API s_API;
	};
}