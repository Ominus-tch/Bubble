#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Bubble {

	namespace Utils {

		static void DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
		{
			// Extract translation
			translation = glm::vec3(transform[3]);

			// Extract scale
			glm::vec3 scaleX = glm::vec3(transform[0]);
			glm::vec3 scaleY = glm::vec3(transform[1]);
			glm::vec3 scaleZ = glm::vec3(transform[2]);

			scale.x = glm::length(scaleX);
			scale.y = glm::length(scaleY);
			scale.z = glm::length(scaleZ);

			// Normalize the rotation matrix
			glm::mat4 rotationMatrix = transform;

			rotationMatrix[0] /= scale.x;
			rotationMatrix[1] /= scale.y;
			rotationMatrix[2] /= scale.z;

			// Extract rotation quaternion
			rotation = glm::eulerAngles(glm::quat_cast(rotationMatrix));
		}

		static glm::vec3 ExtractTranslation(const glm::mat4& transform)
		{
			return glm::vec3(transform[3]);
		}

		static glm::vec3 ExtractRotation(const glm::mat4& transform)
		{
			glm::vec3 scale;
			glm::vec3 scaleX = glm::vec3(transform[0]);
			glm::vec3 scaleY = glm::vec3(transform[1]);
			glm::vec3 scaleZ = glm::vec3(transform[2]);

			scale.x = glm::length(scaleX);
			scale.y = glm::length(scaleY);
			scale.z = glm::length(scaleZ);

			glm::mat4 rotationMatrix = transform;

			rotationMatrix[0] /= scale.x;
			rotationMatrix[1] /= scale.y;
			rotationMatrix[2] /= scale.z;

			// Extract rotation quaternion
			return glm::eulerAngles(glm::quat_cast(rotationMatrix));
		}

		static glm::vec3 ExtractScale(const glm::mat4& transform)
		{
			glm::vec3 scale;
			glm::vec3 scaleX = glm::vec3(transform[0]);
			glm::vec3 scaleY = glm::vec3(transform[1]);
			glm::vec3 scaleZ = glm::vec3(transform[2]);

			scale.x = glm::length(scaleX);
			scale.y = glm::length(scaleY);
			scale.z = glm::length(scaleZ);

			return scale;
		}

	}

	struct Vertex {
		glm::vec3 Position;
		glm::vec4 Color;
	};

	class Mesh
	{
	public:
		static std::vector<Ref<Mesh>> Create(const std::string& path) { return Load(path); }
		static std::vector<Ref<Mesh>> Create(
			const std::vector<Vertex>& verts,
			const std::vector<glm::vec3>& norms,
			const std::vector<glm::vec2>& texCoords,
			const std::vector<uint32_t>& inds)
		{
			return { CreateRef<Mesh>(verts, norms, texCoords, inds) };
		}
		static std::vector<Ref<Mesh>> Load(const std::string& path);

		Mesh() = default;
		Mesh(
			const std::vector<Vertex>& verts,
			const std::vector<uint32_t>& inds)
			: Vertices(verts), Indices(inds) { updateTransform(); }
		Mesh(
			const std::vector<Vertex>& verts,
			const std::vector<glm::vec3>& norms,
			const std::vector<glm::vec2>& texCoords,
			const std::vector<uint32_t>& inds)
			: Vertices(verts), Normals(norms), UVs(texCoords), Indices(inds) { updateTransform(); }
		Mesh(
			const std::vector<Vertex>& verts,
			const std::vector<glm::vec3>& norms,
			const std::vector<glm::vec2>& texCoords,
			const std::vector<uint32_t>& inds,
			const glm::mat4& transform,
			const glm::vec3& translation, 
			const glm::vec3& rotation,
			const glm::vec3& scale)
			: Vertices(verts), Normals(norms), UVs(texCoords), Indices(inds), GlobalTransform(transform), Translation(translation), Rotation(rotation), Scale(scale) {}

		void Clear();

		std::vector<Vertex>		GetVertices()  const { return Vertices;	 }
		std::vector<glm::vec3>  GetNormals()   const { return Normals;	 }
		std::vector<glm::vec2>	GetUVs()	   const { return UVs;		 }
		std::vector<uint32_t>	GetIndices()   const { return Indices;	 }

		uint32_t GetNumVertices()	 const { return Vertices.size();  }
		uint32_t GetNumNormals()	 const { return Normals.size();	  }
		uint32_t GetNumUVs()		 const { return UVs.size();		  }
		uint32_t GetNumIndices()	 const { return Indices.size();	  }

		glm::mat4 GetGlobalTransform() const { return GlobalTransform; }

		glm::vec3 GetTranslation() const { return Utils::ExtractTranslation(GlobalTransform); }
		glm::vec3 GetRotation() const { return Utils::ExtractRotation(GlobalTransform); }
		glm::vec3 GetScale() const { return Utils::ExtractScale(GlobalTransform); }

		void updateTransform(glm::vec3& translation = glm::vec3(0.f), glm::vec3& rotation = glm::vec3(0.f), glm::vec3& scale = glm::vec3(1.f))
		{
			Translation = translation;
			Rotation = rotation;
			Scale = scale;

			GlobalTransform = glm::translate(glm::mat4(1.0f), Translation);

			GlobalTransform = glm::rotate(GlobalTransform, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // Rotation around X-axis
			GlobalTransform = glm::rotate(GlobalTransform, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotation around Y-axis
			GlobalTransform = glm::rotate(GlobalTransform, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotation around Z-axis

			GlobalTransform = glm::scale(GlobalTransform, Scale);
		}

		std::string GetPath() const { return Path; }
	private:
		std::vector<Vertex> Vertices;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec2> UVs;
		std::vector<uint32_t> Indices;

		glm::mat4 GlobalTransform = glm::mat4(1.f);

		glm::vec3 Translation;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		std::string Path = "";
	};

}
