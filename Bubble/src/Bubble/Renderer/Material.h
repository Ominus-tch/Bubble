#pragma once

#include "Shader.h"
#include "Texture.h"

namespace Bubble {

	class Material
	{
	public:
		Material(Ref<Shader> shader)
			: m_Shader(shader) {}

		void SetTexture(const std::string& name, Ref<Texture2D> texture) {
			m_Textures[name] = texture;
		}

		void SetUniform(const std::string& name, const glm::vec2& value) {
			m_UniformsVec2[name] = value;
		}

		void SetUniform(const std::string& name, const glm::vec3& value) {
			m_UniformsVec3[name] = value;
		}

		void SetUniform(const std::string& name, const glm::vec4& value) {
			m_UniformsVec4[name] = value;
		}

		void SetUniform(const std::string& name, const glm::mat4& value) {
			m_UniformsMat4[name] = value;
		}

		void SetUniform(const std::string& name, float value) {
			m_UniformsFloat[name] = value;
		}

		void SetUniform(const std::string& name, int value) {
			m_UniformsInt[name] = value;
		}

		void Apply() {
			m_Shader->Bind();

			// Set all the uniforms
			for (const auto& [name, value] : m_UniformsVec2) {
				m_Shader->SetFloat2(name, value);
			}

			for (const auto& [name, value] : m_UniformsVec3) {
				m_Shader->SetFloat3(name, value);
			}

			for (const auto& [name, value] : m_UniformsVec4) {
				m_Shader->SetFloat4(name, value);
			}

			for (const auto& [name, value] : m_UniformsMat4) {
				m_Shader->SetMat4(name, value);
			}

			for (const auto& [name, value] : m_UniformsFloat) {
				m_Shader->SetFloat(name, value);
			}

			for (const auto& [name, value] : m_UniformsInt) {
				m_Shader->SetInt(name, value);
			}

			// Bind textures
			int textureUnit = 0;
			for (const auto& [name, texture] : m_Textures) {
				texture->Bind(textureUnit);
				m_Shader->SetInt(name, textureUnit);
				textureUnit++;
			}
		}

	private:
		Ref<Shader> m_Shader;
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
		std::unordered_map<std::string, glm::vec2>		m_UniformsVec2;
		std::unordered_map<std::string, glm::vec3>		m_UniformsVec3;
		std::unordered_map<std::string, glm::vec4>		m_UniformsVec4;
		std::unordered_map<std::string, glm::mat4>		m_UniformsMat4;
		std::unordered_map<std::string, float>			m_UniformsFloat;
		std::unordered_map<std::string, int>			m_UniformsInt;
	};

}
