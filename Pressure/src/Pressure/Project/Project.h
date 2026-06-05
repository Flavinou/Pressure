#pragma once

#include <string>
#include <filesystem>

namespace Pressure
{

	struct ProjectConfig
	{
		std::string Name = "Untitled Project";

		std::filesystem::path StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		static const std::filesystem::path& GetProjectDirectory()
		{
			PRS_CORE_ASSERT(ms_ActiveProject);
			return ms_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			PRS_CORE_ASSERT(ms_ActiveProject);
			return ms_ActiveProject->m_ProjectDirectory / ms_ActiveProject->m_Config.AssetDirectory;
		}

		// TODO: Should be handled by some kind of Asset Manager
		static std::filesystem::path GetAssetRelativePath(const std::filesystem::path& path)
		{
			PRS_CORE_ASSERT(ms_ActiveProject);
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return ms_ActiveProject; }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;

		inline static Ref<Project> ms_ActiveProject;
	};

}
