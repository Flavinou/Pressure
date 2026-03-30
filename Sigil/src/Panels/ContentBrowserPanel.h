#pragma once

#include <filesystem>

#include "Pressure/Renderer/Texture.h"

namespace Pressure
{
	
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel(const std::filesystem::path& assetsPath);

		void OnImGuiRender();

	private:
		std::filesystem::path m_CurrentDirectory;
		const std::filesystem::path m_InitialWorkingDirectory;

		Ref<Texture2D> m_DirectoryIcon;
		Ref<Texture2D> m_FileIcon;
	};

}
