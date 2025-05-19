#pragma once

#include "bgpch.h"

#include "Bubble/Utils/MaterialSerializer.h"
#include <yaml-cpp/yaml.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Bubble
{
	Ref<Material> MaterialSerializer::DeserializeMaterial(const std::string& path)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path);
		}
		catch (const YAML::ParserException& e)
		{
			BG_CORE_ERROR("Failed to parse material file: {}", e.what());
			return nullptr;
		}

		auto matNode = data["Material"];
		if (!matNode)
			return nullptr;

		std::string materialPath = matNode["Path"].as<std::string>();
		auto material = Material::Create();

		std::string materialName = matNode["Name"].as<std::string>();
		material->SetName(materialName);

		// Load Shader
		auto shaderNode = matNode["Shader"];
		if (shaderNode && shaderNode["Path"])
		{
			std::string shaderPath = shaderNode["Path"].as<std::string>();
			Ref<Shader> shader = Shader::Create(shaderPath);
			material->SetPath(materialPath);
			material->SetShader(shader);
		}

		// Load Textures
		auto texturesNode = matNode["Textures"];
		if (texturesNode)
		{
			for (auto textureNode : texturesNode)
			{
				std::string texPath = textureNode["Texture"].as<std::string>();
				int unit = textureNode["Unit"] ? textureNode["Unit"].as<int>() : 0;

				Ref<Texture2D> texture = Texture2D::Create(texPath);
				// Assuming Bind will be called later in Apply()

				// You could use texture->Bind(unit) here if necessary
				// Optional: you can validate Width/Height if needed
				material->SetTexture("u_Texture" + std::to_string(unit), texture);
			}
		}

		// Load Uniforms
		auto uniformsNode = matNode["Uniforms"];
		if (uniformsNode)
		{
			if (uniformsNode["vec2"])
			{
				for (auto it : uniformsNode["vec2"])
				{
					auto name = it.first.as<std::string>();
					auto val = it.second.as<std::vector<float>>();
					material->SetUniform(name, glm::vec2(val[0], val[1]));
				}
			}
			if (uniformsNode["vec3"])
			{
				for (auto it : uniformsNode["vec3"])
				{
					auto name = it.first.as<std::string>();
					auto val = it.second.as<std::vector<float>>();
					material->SetUniform(name, glm::vec3(val[0], val[1], val[2]));
				}
			}
			if (uniformsNode["vec4"])
			{
				for (auto it : uniformsNode["vec4"])
				{
					auto name = it.first.as<std::string>();
					auto val = it.second.as<std::vector<float>>();
					material->SetUniform(name, glm::vec4(val[0], val[1], val[2], val[3]));
				}
			}
			if (uniformsNode["mat4"])
			{
				for (auto it : uniformsNode["mat4"])
				{
					auto name = it.first.as<std::string>();
					auto val = it.second.as<std::vector<float>>();
					glm::mat4 mat = glm::make_mat4(val.data());
					material->SetUniform(name, mat);
				}
			}
			if (uniformsNode["float"])
			{
				for (auto it : uniformsNode["float"])
				{
					auto name = it.first.as<std::string>();
					auto val = it.second.as<float>();
					material->SetUniform(name, val);
				}
			}
			if (uniformsNode["int"])
			{
				for (auto it : uniformsNode["int"])
				{
					auto name = it.first.as<std::string>();
					auto val = it.second.as<int>();
					material->SetUniform(name, val);
				}
			}
		}

		return material;
	}

	bool MaterialSerializer::SerializeMaterial(Ref<Material> mat, const std::string& path)
	{
		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;
			{
				out << YAML::Key << "Path" << YAML::Value << mat->GetPath();

				out << YAML::Key << "Shader" << YAML::Value << YAML::BeginMap;
				{
					out << YAML::Key << "Path" << YAML::Value << (mat->GetShader() ? mat->GetShader()->GetPath() : "");
				}

				out << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
				int textureUnit = 0;
				for (int i = 0; i < mat->NumTextures(); i++)
				{
					auto texture = mat->GetTexture(i);
					if (!texture) continue;

					out << YAML::BeginMap;
					out << YAML::Key << "Texture" << YAML::Value << texture->GetPath();
					out << YAML::Key << "Width" << YAML::Value << texture->GetWidth();
					out << YAML::Key << "Height" << YAML::Value << texture->GetHeight();
					out << YAML::Key << "Unit" << YAML::Value << textureUnit;
					out << YAML::EndMap;

					textureUnit++;
				}
				out << YAML::EndSeq;

				// Uniforms
				out << YAML::Key << "Uniforms" << YAML::Value << YAML::BeginMap;

				// vec2
				if (!mat->m_UniformsVec2.empty())
				{
					out << YAML::Key << "vec2" << YAML::Value << YAML::BeginMap;
					for (auto& [name, val] : mat->m_UniformsVec2)
						out << YAML::Key << name << YAML::Value << YAML::Flow << YAML::BeginSeq << val.x << val.y << YAML::EndSeq;
					out << YAML::EndMap;
				}

				// vec3
				if (!mat->m_UniformsVec3.empty())
				{
					out << YAML::Key << "vec3" << YAML::Value << YAML::BeginMap;
					for (auto& [name, val] : mat->m_UniformsVec3)
						out << YAML::Key << name << YAML::Value << YAML::Flow << YAML::BeginSeq << val.x << val.y << val.z << YAML::EndSeq;
					out << YAML::EndMap;
				}

				// vec4
				if (!mat->m_UniformsVec4.empty())
				{
					out << YAML::Key << "vec4" << YAML::Value << YAML::BeginMap;
					for (auto& [name, val] : mat->m_UniformsVec4)
						out << YAML::Key << name << YAML::Value << YAML::Flow << YAML::BeginSeq << val.x << val.y << val.z << val.w << YAML::EndSeq;
					out << YAML::EndMap;
				}

				// mat4
				if (!mat->m_UniformsMat4.empty())
				{
					out << YAML::Key << "mat4" << YAML::Value << YAML::BeginMap;
					for (auto& [name, val] : mat->m_UniformsMat4)
					{
						const float* ptr = glm::value_ptr(val);
						out << YAML::Key << name << YAML::Value << YAML::Flow << YAML::BeginSeq;
						for (int i = 0; i < 16; i++) out << ptr[i];
						out << YAML::EndSeq;
					}
					out << YAML::EndMap;
				}

				// float
				if (!mat->m_UniformsFloat.empty())
				{
					out << YAML::Key << "float" << YAML::Value << YAML::BeginMap;
					for (auto& [name, val] : mat->m_UniformsFloat)
						out << YAML::Key << name << YAML::Value << val;
					out << YAML::EndMap;
				}

				// int
				if (!mat->m_UniformsInt.empty())
				{
					out << YAML::Key << "int" << YAML::Value << YAML::BeginMap;
					for (auto& [name, val] : mat->m_UniformsInt)
						out << YAML::Key << name << YAML::Value << val;
					out << YAML::EndMap;
				}

				out << YAML::EndMap; // End Uniforms
				out << YAML::EndMap; // End Material
				out << YAML::EndMap; // Root

				std::ofstream file(path);
				if (!file.is_open())
					return false;

				file << out.c_str();
				file.close();

				return true;
			}
		}
	}
}
