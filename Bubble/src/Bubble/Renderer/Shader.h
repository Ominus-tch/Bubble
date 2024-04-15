#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Bubble {

	class Shader
	{
	public:
		Shader(const std::string& vertexSrc, const std::string& fragSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetMat4(const std::string& name, const glm::mat4& matrix);
	private:
		uint32_t m_RendererID;
	};

}