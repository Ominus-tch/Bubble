#pragma once

#include "Bubble/Renderer/Texture.h"
#include "Bubble/Renderer/Mesh.h"

#include "Bubble/Core/UUID.h"

#include "Bubble/Scene/SceneCamera.h"


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
		std::vector<Ref<Mesh>> Meshes;
		std::string Path = "";
		bool DrawMesh = true;
		bool DrawWireframe = false;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(const std::string& path) { Meshes = Mesh::Create(path); Path = path; }
		MeshComponent(const std::vector<Vertex>& verts,
			const std::vector<glm::vec3>& norms,
			const std::vector<glm::vec2>& texCoords,
			const std::vector<uint32_t>& inds)
		{
			Meshes = Mesh::Create(verts, norms, texCoords, inds);
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
		}

		void Clear()
		{
			Path = "";
			for (auto& mesh : Meshes)
			{
				mesh->Clear();
			}

			Meshes.clear();
		}

		void AddMesh(Ref<Mesh> mesh)
		{
			Meshes.push_back(mesh);
		}

		void AddMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
		{
			Meshes.push_back(CreateRef<Mesh>(vertices, indices));
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

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteRendererComponent,
		NativeScriptComponent, MeshComponent, CameraComponent,
		ScriptComponent>;
}
