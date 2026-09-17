#include "prspch.h"
#include "ContentBrowserPanel.h"

#include "Pressure/Asset/TextureImporter.h"
#include "Pressure/Project/Project.h"
#include "Pressure/UI/UI.h"

#include <imgui/imgui.h>

namespace Pressure
{
	
	ContentBrowserPanel::ContentBrowserPanel()
		: m_BaseDirectory(Project::GetAssetDirectory())
		, m_CurrentDirectory(m_BaseDirectory)
	{
		m_TreeNodes.emplace_back(".", 0);

		m_DirectoryIcon = TextureImporter::LoadTexture2D("resources/icons/folder_icon.png");
		m_FileIcon = TextureImporter::LoadTexture2D("resources/icons/file_icon.png");

		RefreshAssetTree();

		m_Mode = Mode::FileSystem;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		const char* label = m_Mode == Mode::Asset ? "Asset Explorer" : "File Explorer";
		if (ImGui::Button(label))
		{
			m_Mode = m_Mode == Mode::Asset ? Mode::FileSystem : Mode::Asset;
		}

		if (m_CurrentDirectory != std::filesystem::path(m_BaseDirectory))
		{
			ImGui::SameLine();
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

		if (m_Mode == Mode::Asset)
		{
			TreeNode* node = &m_TreeNodes[0];

			auto currentDir = std::filesystem::relative(m_CurrentDirectory, Project::GetAssetDirectory());
			for (const auto& part : currentDir)
			{
				if (node->Path == currentDir)
					break;

				if (node->Children.find(part) != node->Children.end())
				{
					node = &m_TreeNodes[node->Children[part]];
					continue;
				}

				// Can't find path
				PRS_CORE_ASSERT(false);
			}

			for (const auto& [item, treeNodeIndex] : node->Children)
			{
				bool isDirectory = std::filesystem::is_directory(Project::GetAssetDirectory() / item);
				std::string itemStr = item.generic_string();

				ImGui::PushID(itemStr.c_str());
				Ref<Texture2D> icon = isDirectory ? m_DirectoryIcon : m_FileIcon;
				UI::ScopedStyleColor buttonColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon->GetRendererID())
					, { thumbnailSize, thumbnailSize }
					, { 0, 1 }
					, { 1, 0 });

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete"))
					{
						PRS_CORE_ASSERT(false, "Not implemented");
					}
					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource())
				{
					AssetHandle handle = m_TreeNodes[treeNodeIndex].Handle;
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(AssetHandle));
					ImGui::EndDragDropSource();
				}

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					if (isDirectory)
					{
						m_CurrentDirectory /= item.filename();
					}
				}

				ImGui::TextWrapped(itemStr.c_str());
				ImGui::NextColumn();
				ImGui::PopID();
			}
		}
		else
		{

			for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				const auto& path = directoryEntry.path();
				std::string filenameString = path.filename().string();

				ImGui::PushID(filenameString.c_str());
				Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;

				{
					UI::ScopedStyleColor buttonColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
					ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon->GetRendererID())
						, { thumbnailSize, thumbnailSize }
						, { 0, 1 }
					, { 1, 0 });

					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Import"))
						{
							auto relativePath = std::filesystem::relative(path, Project::GetAssetDirectory());
							Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);
							RefreshAssetTree();
						}
						ImGui::EndPopup();
					}
				}

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
		}

		ImGui::Columns(1);

		// Optional settings for thumbnail size and padding
		{
			ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 32, 256);
			ImGui::SliderFloat("Padding", &padding, 0, 32);
		}

		ImGui::End();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		const auto& assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
		for (const auto& [handle, metadata] : assetRegistry)
		{
			uint32_t currentNodeIndex = 0;

			for (const auto& path : metadata.FilePath)
			{
				auto it = m_TreeNodes[currentNodeIndex].Children.find(path.generic_string());
				if (it != m_TreeNodes[currentNodeIndex].Children.end())
				{
					currentNodeIndex = it->second;
					continue;
				}

				TreeNode newNode(path, handle);
				newNode.Parent = currentNodeIndex;
				m_TreeNodes.push_back(newNode);

				m_TreeNodes[currentNodeIndex].Children[path] = m_TreeNodes.size() - 1;
				currentNodeIndex = m_TreeNodes.size() - 1;
			}
		}
	}
}
