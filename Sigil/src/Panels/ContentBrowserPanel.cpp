#include "prspch.h"
#include "ContentBrowserPanel.h"

#include <imgui/imgui.h>

ContentBrowserPanel::ContentBrowserPanel(const std::filesystem::path& assetsPath)
	: m_CurrentDirectory(assetsPath)
	, m_InitialWorkingDirectory(assetsPath)
{
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

	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, m_CurrentDirectory);
		std::string filenameString = relativePath.filename().string();

		if (directoryEntry.is_directory())
		{
			if (ImGui::Button(filenameString.c_str()))
			{
				m_CurrentDirectory /= path.filename();
			}
		}
		else
		{
			ImGui::Text(filenameString.c_str());
		}
	}

	ImGui::End();
}
