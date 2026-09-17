#pragma once

#include <string>
#include <filesystem>

#include "Pressure/Asset/RuntimeAssetManager.h"
#include "Pressure/Asset/EditorAssetManager.h"

namespace Pressure
{
	class AssetManagerBase;

	struct ProjectConfig
	{
		std::string Name = "Untitled Project";

		AssetHandle StartScene;

		std::filesystem::path AssetDirectory;
		std::filesystem::path AssetRegistryPath; // Relative to AssetDirectory
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

		static std::filesystem::path GetAssetRegistryPath()
		{
			PRS_CORE_ASSERT(ms_ActiveProject);
			return GetAssetDirectory() / ms_ActiveProject->m_Config.AssetRegistryPath;
		}

		// TODO: Should be handled by some kind of Asset Manager
		static std::filesystem::path GetAssetRelativePath(const std::filesystem::path& path)
		{
			PRS_CORE_ASSERT(ms_ActiveProject);
			return GetAssetDirectory() / path;
		}

		ProjectConfig& GetConfig() { return m_Config; }

		static Ref<Project> GetActive() { return ms_ActiveProject; }
		Ref<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
		Ref<RuntimeAssetManager> GetRuntimeAssetManager() const { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManager); }
		Ref<EditorAssetManager> GetEditorAssetManager() const { return std::static_pointer_cast<EditorAssetManager>(m_AssetManager); }

		static Ref<Project> New();
		static Ref<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		Ref<AssetManagerBase> m_AssetManager;

		inline static Ref<Project> ms_ActiveProject;
	};

}
