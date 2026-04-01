#include "prspch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

namespace Pressure
{
	
	ContentBrowserPanel::ContentBrowserPanel(const std::filesystem::path& assetsPath)
		: m_CurrentDirectory(assetsPath)
		, m_InitialWorkingDirectory(assetsPath)
	{
		m_DirectoryIcon = Texture2D::Create("resources/icons/folder_icon.png");
		m_FileIcon = Texture2D::Create("resources/icons/file_icon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(m_InitialWorkingDirectory))
		{
			if (ImGui::Button(".."))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 128.0f;
		float cellSize = thumbnailSize + padding;

		// The idea is to show the directory and file icons as buttons
		// If the user clicks on them, we navigate into the directory. 
		// We also want to show the filename below the icon. 
		// We need to calculate the available space for each item to properly layout the icons and text.
		auto availableSpace = ImGui::GetContentRegionAvail();
		const int columnCount = std::max(1, static_cast<int>(availableSpace.x / cellSize));

		ImGui::Columns(columnCount, nullptr, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, m_InitialWorkingDirectory);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon->GetRendererID())
				, { thumbnailSize, thumbnailSize }
				, { 0, 1 }
			, { 1, 0 });

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= path.filename();
				}
			}

			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);

		// Optional settings for thumbnail size and padding
		{
			ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 32, 256);
			ImGui::SliderFloat("Padding", &padding, 0, 32);
		}

		ImGui::End();
	}

}
