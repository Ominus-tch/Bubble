#include "bgpch.h"
#include "ContentBrowserPanel.h"

#include "Bubble/Project/Project.h"

#include <imgui/imgui.h>

namespace Bubble {

	ContentBrowserPanel::ContentBrowserPanel()
		: m_BaseDirectory(Project::GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		BG_INFO("Current Directory: {0}", m_CurrentDirectory.string().c_str());
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		bool anyItemHovered = false;

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.filename().string();

			bool isDir = directoryEntry.is_directory();

			//ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = isDir ? m_DirectoryIcon : m_FileIcon;

			if (path.extension().string() == ".png")
			{
				std::string pathStr = path.string();
				if (m_TextureIcons.find(pathStr) == m_TextureIcons.end())
				{
					icon = Texture2D::Create(pathStr);
					m_TextureIcons[pathStr] = icon;
				}
				else
				{
					icon = m_TextureIcons[pathStr];
				}
			}


			if (m_SelectedEntry == path)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 1, 1, 0.2));
			else
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			ImGui::ImageButton(filenameString.c_str(), (ImTextureID)icon->GetRendererID(), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				std::filesystem::path relativePath(path);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::Image((ImTextureID)icon->GetRendererID(), { thumbnailSize * 0.2f, thumbnailSize * 0.2f }, { 0, 1 }, { 1, 0 });
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			bool hovered = ImGui::IsItemHovered();
			anyItemHovered |= hovered;

			if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedEntry = path;
			}


			if (hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();
			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			//ImGui::PopID();
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !anyItemHovered)
		{
			m_SelectedEntry.clear();
		}

		ImGui::Columns(1);

		//ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
		//ImGui::SliderFloat("Padding", &padding, 0, 32);

		// TODO: status bar
		ImGui::End();
	}

}
