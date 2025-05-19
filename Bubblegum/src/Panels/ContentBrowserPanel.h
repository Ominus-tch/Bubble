#pragma once

#include "Bubble/Renderer/Texture.h"

#include <filesystem>

namespace Bubble {

	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

		std::string GetSelectedEntry() const { return m_SelectedEntry.string(); }
		void SetSelectedEntry(std::string newPath = "") { m_SelectedEntry = newPath; }

		std::unordered_map<std::string, Ref<Texture2D>> GetAssetIcons() const {
			return m_TextureIcons;
		}
	private:
		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_SelectedEntry;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
		std::unordered_map<std::string, Ref<Texture2D>> m_TextureIcons;
	};

}
