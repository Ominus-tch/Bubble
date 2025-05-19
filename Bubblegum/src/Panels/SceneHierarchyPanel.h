#pragma once

#include "Bubble/Core/Base.h"
#include "Bubble/Scene/Scene.h"
#include "Bubble/Scene/Entity.h"

#include <map>

namespace Bubble {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);

		void SetSelectedDirectoryEntry(const std::string& newPath) { 
			m_SelectedDirectoryEntry = newPath; 

			if (m_SelectionContext)
				m_SelectionPending[m_SelectionContext] = false;
		}

		void SetAssetIcons(const std::unordered_map<std::string, Ref<Texture2D>>& icons) { m_AssetIcons = icons; }
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
		void DrawDirectoryEntry(const std::string& directoryPath);
	private:
		std::map<uint32_t, bool> m_SelectionPending;

		Ref<Scene> m_Context;
		Entity m_SelectionContext;

		std::string m_SelectedDirectoryEntry;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		std::unordered_map<std::string, Ref<Texture2D>> m_AssetIcons;
	};

}
