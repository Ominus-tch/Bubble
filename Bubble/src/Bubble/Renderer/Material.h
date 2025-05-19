#pragma once

#include "Shader.h"
#include "Texture.h"

namespace Bubble {

	// Temp
	struct _UniformsBuffer
	{
		glm::vec4 val;
	};

	class Material
	{
	public:
		Material() {}
		Material(const std::string& path)
			: m_Path(path) {}
		Material(Ref<Shader> shader)
			: m_Shader(shader) {}
		 
		static Ref<Material> Create();
		static Ref<Material> Create(const std::string& path);
		static Ref<Material> Create(Ref<Shader> shader);

		void SetShader(Ref<Shader> shader)
		{
			m_Shader = shader;
		}

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

		int NumTextures()
		{
			return m_Textures.size();
		}

		bool HasTexture(const std::string& name)
		{
			return m_Textures.find(name) != m_Textures.end() && m_Textures[name] != nullptr;
		}

		Ref<Texture2D> GetTexture(const std::string& name)
		{
			if (!HasTexture(name)) return nullptr;

			return m_Textures[name];
		}

		Ref<Texture2D> GetTexture(int index)
		{
			if (index < 0 || index >= m_Textures.size()) {
				return nullptr;
			}

			auto it = std::next(m_Textures.begin(), index);
			return it->second;
		}

		std::string GetPath() const { return m_Path; }
		void SetPath(const std::string& path)  { m_Path = path; }

		std::string GetName() const { return m_Name; }
		void SetName(const std::string& name) { m_Name = name; }

		void Apply(uint32_t uniformBufferIndex, uint32_t textureSlotIndex) {
			//UniformsBuffer = nullptr;
			//UniformsBuffer = UniformBuffer::Create(sizeof(_UniformsBuffer), uniformBufferIndex);
			if (m_Shader)
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
			int textureUnit = textureSlotIndex;
			for (const auto& [name, texture] : m_Textures) {
				texture->Bind(textureUnit);
				m_Shader->SetInt(name, textureUnit);
				textureUnit++;
			}
		}

		void Clear() {
			// Clear all uniform maps
			m_UniformsVec2.clear();
			m_UniformsVec3.clear();
			m_UniformsVec4.clear();
			m_UniformsMat4.clear();
			m_UniformsFloat.clear();
			m_UniformsInt.clear();

			// Clear textures and associated texture units
			m_Textures.clear();
			m_TextureUnits.clear();

			// Reset shader and metadata if desired
			m_Shader = nullptr;
			m_Name.clear();
			m_Path.clear();
		}

		Ref<Shader> GetShader() const { return m_Shader; }

		std::unordered_map<std::string, glm::vec2>		m_UniformsVec2;
		std::unordered_map<std::string, glm::vec3>		m_UniformsVec3;
		std::unordered_map<std::string, glm::vec4>		m_UniformsVec4;
		std::unordered_map<std::string, glm::mat4>		m_UniformsMat4;
		std::unordered_map<std::string, float>			m_UniformsFloat;
		std::unordered_map<std::string, int>			m_UniformsInt;

	private:
		Ref<UniformBuffer> UniformsBuffer;
		Ref<Shader> m_Shader = nullptr;
		std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
		std::unordered_map<std::string, int> m_TextureUnits;

		std::string m_Name, m_Path;
	};

	class MaterialLibrary
	{
	public:
		static void Add(const std::string& name, const Ref<Material>& material, bool overwrite = false);
		static void Add(const Ref<Material>& material);
		static Ref<Material> Load(const std::string& filepath);
		static Ref<Material> Load(const std::string& name, const std::string& filepath);

		static bool Has(const std::string& name);
		static Ref<Material> Get(const std::string& name);
		static std::unordered_map<std::string, Ref<Material>> GetMaterials();
		static void Remove(const std::string& name);

		static bool Reload(const std::string& name);
	private:
		static std::unordered_map<std::string, Ref<Material>> m_Materials;
	};

}
