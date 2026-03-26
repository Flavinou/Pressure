#pragma once

#include <filesystem>

class ContentBrowserPanel
{
public:
	ContentBrowserPanel(const std::filesystem::path& assetsPath);

	void OnImGuiRender();

private:
	std::filesystem::path m_CurrentDirectory;
	const std::filesystem::path m_InitialWorkingDirectory;
};
