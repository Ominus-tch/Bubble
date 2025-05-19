#pragma once

#include "Bubble/Scene/SceneCamera.h"

#include "Bubble/Core/UUID.h"
#include "Bubble/Renderer/Texture.h"
#include "Bubble/Renderer/Mesh.h"
#include "Bubble/Renderer/Material.h"
#include "Bubble/Utils/MaterialSerializer.h"
//#include "Bubble/Renderer/Font.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Bubble {

	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& id) : ID(id) {}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		std::tuple <glm::vec3, glm::vec3, glm::vec3> GetDirectionVectors() const
		{
			glm::mat4 transform = GetTransform();
			glm::vec3 forward = glm::normalize(glm::vec3(transform[2]));
			glm::vec3 up = glm::normalize(glm::vec3(transform[1]));
			glm::vec3 right = glm::normalize(glm::vec3(transform[0]));
			return std::make_tuple(forward, up, right);
		}

		glm::vec3 GetForward() const
		{
			// Extract forward direction from the rotation matrix
			glm::mat4 transform = GetTransform();
			return glm::normalize(glm::vec3(transform[2])); // Z direction
		}

		glm::vec3 GetUp() const
		{
			// Extract up direction from the rotation matrix
			glm::mat4 transform = GetTransform();
			return glm::normalize(glm::vec3(transform[1])); // Y direction
		}

		glm::vec3 GetRight() const
		{
			// Extract right direction from the rotation matrix
			glm::mat4 transform = GetTransform();
			return glm::normalize(glm::vec3(transform[0])); // X direction
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct MeshComponent
	{
		std::vector<Ref<Mesh>> Meshes; // Really submeshes
		std::vector<Ref<Material>> Materials;
		std::string Path = "";
		bool DrawMesh = true;
		bool DrawWireframe = true;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path) { Meshes = Mesh::Create(path); Path = path; OnMeshesLoaded(); }
		MeshComponent(const std::vector<Vertex>& verts,
			const std::vector<glm::vec3>& norms,
			const std::vector<glm::vec2>& texCoords,
			const std::vector<uint32_t>& inds)
		{
			Meshes = Mesh::Create(verts, norms, texCoords, inds);
			OnMeshesLoaded();
		}

		void Load(const std::string& path)
		{
			if (std::filesystem::path(path).extension().string() != ".fbx")
			{
				BG_WARN("Could not load {0} - not a model file", path);
				return;
			}

			Clear();
			Path = path;
			Meshes = Mesh::Create(path);
			OnMeshesLoaded();
		}

		void OnMeshesLoaded()
		{
			if (Materials.size() == 0)
			{
				Ref<Material> mat = Material::Create("assets/materials/DefaultGray.bmat");
				BG_CORE_INFO(mat->GetShader()->GetName());
				Materials.push_back(mat);
			}
		}

		void Load()
		{
			if (!Path.empty())
				Load(Path);
			else
			{
				BG_CORE_ERROR("Error at MeshComponent::Load(): Please provide a path");
			}
		}

		void Clear()
		{
			Path = "";
			for (auto& mesh : Meshes)
			{
				mesh->Clear();
			}

			for (auto& mat : Materials)
			{
				mat->Clear();
			}

			Meshes.clear();
			Materials.clear();
		}

		void AddMesh(Ref<Mesh> mesh)
		{
			Meshes.push_back(mesh);
			OnMeshesLoaded();
		}

		void AddMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
		{
			AddMesh(CreateRef<Mesh>(vertices, indices));
		}

		Ref<Material> AddMaterial(const std::string& filepath)
		{
			BG_INFO("Adding Material {0} to {1}", filepath, Path);
			if (filepath.substr(filepath.find_last_of('.') + 1) == "bmat")
			{
				Ref<Material> mat = MaterialSerializer::DeserializeMaterial(filepath);
				AddMaterial(mat);
				return mat;
			}
			else if (filepath.substr(filepath.find_last_of('.') + 1) == "png")
			{
				Ref<Texture2D> texture = Texture2D::Create(filepath);
				return AddMaterial(filepath, texture);
			}
			else
			{
				BG_WARN("Not valid material or texture file");
			}

			return nullptr;
		}

		bool SwitchMaterial(const std::string& oldMaterialPath, const std::string& filepath)
		{

			Ref<Material> mat = nullptr;

			size_t extPos = filepath.find_last_of('.');
			if (extPos == std::string::npos)
			{
				BG_WARN("File has no extension: {0}", filepath);
				return false;
			}

			std::string ext = filepath.substr(extPos + 1);
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			if (ext == "bmat")
			{
				mat = MaterialSerializer::DeserializeMaterial(filepath);
			}
			else if (ext == "png")
			{
				Ref<Texture2D> texture = Texture2D::Create(filepath);
				mat = Material::Create();
				mat->SetName(std::filesystem::path(filepath).filename().string());
				mat->SetTexture(filepath, texture);
			}
			else
			{
				BG_WARN("Not valid material or texture file");
			}

			if (mat)
			{
				int index = -1;
				int i = 0;
				for (const auto& _mat : Materials)
				{
					BG_INFO(i);
					BG_INFO(oldMaterialPath);
					BG_INFO(_mat->GetPath());
					BG_INFO(_mat->GetName());
					if (_mat->GetPath() == oldMaterialPath || _mat->GetName() == oldMaterialPath)
					{
						index = i;
						break;
					}

					i++;
				}

				if (index != -1)
				{
					BG_INFO(index);
					Materials[index]->Clear();
					Materials[index] = mat;
					
					return true;
				}
			}

			return false;
		}

		void AddMaterial(Ref<Material> material)
		{
			Materials.push_back(material);
		}

		Ref<Material> AddMaterial(const std::string& name, Ref<Texture2D> texture)
		{
			auto material = Material::Create();
			material->SetTexture(name, texture);
			AddMaterial(material);
			return material;
		}
	};

	struct CameraComponent
	{
		Bubble::SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent
	{
		std::string ClassName;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
	};

	// Forward declaration
	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	/*struct TextComponent
	{
		std::string TextString;
		Ref<Font> FontAsset = Font::GetDefault();
		glm::vec4 Color{ 1.0f };
		float Kerning = 0.0f;
		float LineSpacing = 0.0f;
	};*/

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		NativeScriptComponent, MeshComponent,
		CameraComponent, ScriptComponent>;
}
