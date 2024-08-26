#include "bgpch.h"
#include "Mesh.h"

#include <ofbx.h>

namespace Bubble {

	namespace Utils {

		static bool saveAsOBJ(ofbx::IScene& scene, const char* path)
		{
			FILE* fp = fopen(path, "wb");
			if (!fp) return false;
			int indices_offset = 0;
			int mesh_count = scene.getMeshCount();

			// output unindexed geometry
			for (int mesh_idx = 0; mesh_idx < mesh_count; ++mesh_idx) {
				const ofbx::Mesh& mesh = *scene.getMesh(mesh_idx);
				const ofbx::GeometryData& geom = mesh.getGeometryData();
				const ofbx::Vec3Attributes positions = geom.getPositions();
				const ofbx::Vec3Attributes normals = geom.getNormals();
				const ofbx::Vec2Attributes uvs = geom.getUVs();

				// each ofbx::Mesh can have several materials == partitions
				for (int partition_idx = 0; partition_idx < geom.getPartitionCount(); ++partition_idx) {
					fprintf(fp, "o obj%d_%d\ng grp%d\n", mesh_idx, partition_idx, mesh_idx);
					const ofbx::GeometryPartition& partition = geom.getPartition(partition_idx);

					// partitions most likely have several polygons, they are not triangles necessarily, use ofbx::triangulate if you want triangles
					for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx) {
						const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygon_idx];

						for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
							ofbx::Vec3 v = positions.get(i);
							fprintf(fp, "v %f %f %f\n", v.x, v.y, v.z);
						}

						bool has_normals = normals.values != nullptr;
						if (has_normals) {
							// normals.indices might be different than positions.indices
							// but normals.get(i) is normal for positions.get(i)
							for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
								ofbx::Vec3 n = normals.get(i);
								fprintf(fp, "vn %f %f %f\n", n.x, n.y, n.z);
							}
						}

						bool has_uvs = uvs.values != nullptr;
						if (has_uvs) {
							for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
								ofbx::Vec2 uv = uvs.get(i);
								fprintf(fp, "vt %f %f\n", uv.x, uv.y);
							}
						}
					}

					for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx) {
						const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygon_idx];
						fputs("f ", fp);
						for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
							fprintf(fp, "%d ", 1 + i + indices_offset);
						}
						fputs("\n", fp);
					}

					indices_offset += positions.count;
				}
			}
			fclose(fp);
			return true;
		}

		static ofbx::IScene* initOpenFBX(const char* filepath) {
			static char s_TimeString[256];
			FILE* fp = fopen(filepath, "rb");

			if (!fp) return false;

			fseek(fp, 0, SEEK_END);
			long file_size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			auto* content = new ofbx::u8[file_size];
			fread(content, 1, file_size, fp);

			// Ignoring certain nodes will only stop them from being processed not tokenised (i.e. they will still be in the tree)
			ofbx::LoadFlags flags =
				//		ofbx::LoadFlags::IGNORE_MODELS |
				ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
				ofbx::LoadFlags::IGNORE_CAMERAS |
				ofbx::LoadFlags::IGNORE_LIGHTS |
				//ofbx::LoadFlags::IGNORE_TEXTURES |
				ofbx::LoadFlags::IGNORE_SKIN |
				ofbx::LoadFlags::IGNORE_BONES |
				ofbx::LoadFlags::IGNORE_PIVOTS |
				//ofbx::LoadFlags::IGNORE_MATERIALS |
				ofbx::LoadFlags::IGNORE_POSES |
				ofbx::LoadFlags::IGNORE_VIDEOS |
				ofbx::LoadFlags::IGNORE_LIMBS |
				// ofbx::LoadFlags::IGNORE_MESHES |
				ofbx::LoadFlags::IGNORE_ANIMATIONS;

			auto* scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);

			if (!scene) {
				OutputDebugStringA(ofbx::getError());
			}
			delete[] content;
			fclose(fp);

			return scene;
		}

		static glm::mat4 ConvertToGLM(const ofbx::DMatrix& matrix) {
			glm::mat4 result;

			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					result[i][j] = static_cast<float>(matrix.m[i * 4 + j]);
				}
			}

			return result;
		}

		static glm::vec3 ConvertToGLM(const ofbx::DVec3& vec) {
			glm::vec3 result = { vec.x, vec.y, vec.z };
			return result;
		}

	}

	void Mesh::Clear()
	{
		Vertices.clear();
		Normals.clear();
		UVs.clear();
		Indices.clear();
		GlobalTransform = glm::mat4(1.f);

		Path = "";
	}

	std::vector<Ref<Mesh>> Mesh::Load(const std::string& path)
	{
		const ofbx::IScene* scene = Utils::initOpenFBX(path.c_str());

		std::vector<Ref<Mesh>> meshes;

		if (!scene)
			return meshes;

		const int meshCount = scene->getMeshCount();
		for (int meshIdx = 0; meshIdx < meshCount; meshIdx++) {
			const ofbx::Mesh* mesh = scene->getMesh(meshIdx);
			const ofbx::GeometryData& geomData = mesh->getGeometryData();

			std::vector<Vertex> _vertices;
			std::vector<glm::vec3> _normals;
			std::vector<glm::vec2> _uvs;
			std::vector<uint32_t> _indices;
			glm::mat4& _transform = Utils::ConvertToGLM(mesh->getGlobalTransform());

			glm::vec3 _position = Utils::ConvertToGLM(mesh->getLocalTranslation());
			glm::vec3 _rotation = Utils::ConvertToGLM(mesh->getLocalRotation());
			glm::vec3 _scale = Utils::ConvertToGLM(mesh->getLocalScaling());

			ofbx::Vec3Attributes positions = geomData.getPositions();
			ofbx::Vec3Attributes normals = geomData.getNormals();
			ofbx::Vec2Attributes uvs = geomData.getUVs(0);
			ofbx::Vec4Attributes colors = geomData.getColors();
			//ofbx::Vec3Attributes tangents = geomData.getTangents();

			for (int c = 0; c < colors.count; c++)
			{
				ofbx::Vec4 color = colors.get(c);
				glm::vec4 glmColor(color.x, color.y, color.z, color.w);

				BG_INFO(glm::to_string(glmColor));
			}

			for (int v = 0; v < positions.count; v++) {
				ofbx::Vec3 position = positions.get(v);
				glm::vec3 glmPosition(position.x, position.y, position.z);

				ofbx::Vec4 color = colors.count > v ? colors.get(v) : ofbx::Vec4{ 1, 1, 1, 1 };
				glm::vec4 glmColor(color.x, color.y, color.z, color.w);

				_vertices.push_back({ glmPosition, glmColor });
			}

			for (int n = 0; n < normals.count; n++)
			{
				ofbx::Vec3 normal = normals.get(n);
				glm::vec3 glmNormal(normal.x, normal.y, normal.z);

				_normals.push_back(glmNormal);
			}

			for (int u = 0; u < uvs.count; u++)
			{
				ofbx::Vec2 uv = uvs.get(u);
				glm::vec2 glmUV(uv.x, uv.y);

				_uvs.push_back(glmUV);
			}

			int partitionCount = geomData.getPartitionCount();

			for (int p = 0; p < partitionCount; ++p) {
				const ofbx::GeometryPartition partition = geomData.getPartition(p);

				for (int polyIdx = 0; polyIdx < partition.polygon_count; ++polyIdx) {
					const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polyIdx];

					for (int i = 1; i < polygon.vertex_count - 1; ++i) {
						int v0 = polygon.from_vertex;
						int v1 = polygon.from_vertex + i;
						int v2 = polygon.from_vertex + i + 1;

						_indices.push_back(static_cast<uint32_t>(v0));
						_indices.push_back(static_cast<uint32_t>(v1));
						_indices.push_back(static_cast<uint32_t>(v2));
					}
				}
			}

			meshes.push_back(CreateRef<Mesh>(_vertices, _normals, _uvs, _indices, _transform, _position, _rotation, _scale));
		}

		return meshes;
	}

}
