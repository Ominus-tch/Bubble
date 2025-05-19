#include "bgpch.h"
#include "Material.h"

#include "Bubble/Utils/MaterialSerializer.h";

namespace Bubble {

	std::unordered_map<std::string, Ref<Material>> MaterialLibrary::m_Materials;

	Ref<Material> Material::Create() { // Generate Identification
		return CreateRef<Material>();
	}

	Ref<Material> Material::Create(const std::string& filepath)
	{
		if (MaterialLibrary::Has(filepath))
			return MaterialLibrary::Get(filepath);

		Ref<Material> mat = MaterialSerializer::DeserializeMaterial(filepath);
		MaterialLibrary::Add(filepath, mat);

		return mat;

		BG_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<Material> Material::Create(Ref<Shader> shader) { // Generate Identification
		return CreateRef<Material>(shader);
	}

	//

	void MaterialLibrary::Add(const std::string& name, const Ref<Material>& material, bool overwrite)
	{
		if (!overwrite)
			BG_CORE_ASSERT(Has(name), "Material already exists!");

		material->SetName(name);
		m_Materials[name] = material;
	}

	void MaterialLibrary::Add(const Ref<Material>& material)
	{
		auto& name = material->GetName();
		Add(name, material);
	}

	Ref<Material> MaterialLibrary::Load(const std::string& filepath)
	{
		auto material = Material::Create(filepath);
		Add(material);
		return material;
	}

	Ref<Material> MaterialLibrary::Load(const std::string& pathOrName, const std::string& filepath)
	{
		auto material = Material::Create(filepath);
		Add(pathOrName, material);
		return material;
	}

	bool MaterialLibrary::Has(const std::string& pathOrName)
	{
		return m_Materials.find(pathOrName) != m_Materials.end();
	}

	Ref<Material> MaterialLibrary::Get(const std::string& pathOrName)
	{
		if (Has(pathOrName))
			return m_Materials[pathOrName];
		return nullptr;
	}

	std::unordered_map<std::string, Ref<Material>> MaterialLibrary::GetMaterials()
	{
		return m_Materials;
	}

	bool MaterialLibrary::Reload(const std::string& pathOrName)
	{
		if (!Has(pathOrName)) return false;

		auto path = m_Materials[pathOrName]->GetPath();

		auto reloaded = Material::Create(path);
		if (!reloaded) return false;

		m_Materials[pathOrName] = reloaded;
		return true;
	}

	void MaterialLibrary::Remove(const std::string& pathOrName)
	{
		m_Materials.erase(pathOrName);
	}

}
