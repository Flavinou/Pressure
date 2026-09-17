#include "prspch.h"
#include "EditorAssetManager.h"

#include "Pressure/Asset/AssetImporter.h"
#include "Pressure/Asset/AssetMetadata.h"
#include "Pressure/Project/Project.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Pressure
{

	namespace
	{
		std::map<std::filesystem::path, AssetType> s_AssetExtensionMap = {
			{ ".prs", AssetType::Scene },
			{ ".png", AssetType::Texture2D },
			{ ".jpg", AssetType::Texture2D },
			{ ".jpeg", AssetType::Texture2D },
			{ ".bmp", AssetType::Texture2D },
			{ ".tga", AssetType::Texture2D },
			{ ".hdr", AssetType::Texture2D }
		};

		AssetType GetAssetTypeFromFileExtension(const std::filesystem::path& extension)
		{
			if (s_AssetExtensionMap.find(extension) == s_AssetExtensionMap.end())
			{
				PRS_CORE_WARN("Unknown asset type for file extension: {0}", extension.string());
				return AssetType::None;
			}

			return s_AssetExtensionMap.at(extension);
		}
	}
	
	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
	{
		// 1. Check if the handle is valid
		if (!IsAssetHandleValid(handle))
		{
			return nullptr;
		}

		// 2. Check if the asset needs loading (if not loaded, load it)
		Ref<Asset> asset;
		if (IsAssetLoaded(handle))
		{
			asset = m_LoadedAssets.at(handle);
		}
		else
		{
			// Load the asset
			const AssetMetadata& metadata = GetMetadata(handle);
			asset = AssetImporter::Import(handle, metadata);
			if (!asset)
			{
				PRS_CORE_ERROR("Failed to import asset with handle {0}", handle);
				return nullptr;
			}

			m_LoadedAssets[handle] = asset;
		}

		// 3. Return the asset
		return asset;
	}

	void EditorAssetManager::ImportAsset(const std::filesystem::path& filePath)
	{
		AssetHandle handle; // Generate a new unique handle for the asset
		AssetMetadata metadata;
		metadata.FilePath = filePath;
		metadata.Type = GetAssetTypeFromFileExtension(filePath.extension());
		PRS_CORE_ASSERT(metadata.Type != AssetType::None);

		Ref<Asset> asset = AssetImporter::Import(handle, metadata);
		if (!asset)
		{
			PRS_CORE_ERROR("Failed to import asset from path '{0}'", filePath.string());
			return;
		}

		asset->m_Handle = handle;
		m_LoadedAssets[handle] = asset;
		m_AssetRegistry[handle] = metadata;

		SerializeAssetRegistry();
	}

	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return handle != 0 && m_AssetRegistry.find(handle) != m_AssetRegistry.end();
	}

	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_LoadedAssets.find(handle) != m_LoadedAssets.end();
	}

	AssetType EditorAssetManager::GetAssetType(AssetHandle handle) const
	{
		if (!IsAssetHandleValid(handle))
		{
			return AssetType::None;
		}

		return m_AssetRegistry.at(handle).Type;
	}

	const std::filesystem::path& EditorAssetManager::GetFilePath(AssetHandle handle) const
	{
		return GetMetadata(handle).FilePath;
	}

	void EditorAssetManager::SerializeAssetRegistry()
	{
		auto path = Project::GetAssetRegistryPath();

		YAML::Emitter out;
		{
			out << YAML::BeginMap; // Root
			out << YAML::Key << "AssetRegistry" << YAML::Value;

			out << YAML::BeginSeq;
			for (const auto& [handle, metadata] : m_AssetRegistry)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Handle" << YAML::Value << handle;
				std::string filePathStr = metadata.FilePath.generic_string();
				out << YAML::Key << "FilePath" << YAML::Value << filePathStr;
				out << YAML::Key << "Type" << YAML::Value << AssetTypeToString(metadata.Type);
				out << YAML::EndMap;
			}

			out << YAML::EndSeq;
			out << YAML::EndMap; // Root
		}

		std::ofstream fout(path);
		fout << out.c_str();
	}

	bool EditorAssetManager::DeserializeAssetRegistry()
	{
		auto path = Project::GetAssetRegistryPath();

		YAML::Node data;
		try
		{
			data = YAML::LoadFile(path.string());
		}
		catch (YAML::Exception& e)
		{
			PRS_CORE_ERROR("Failed to load asset registry from path '{0}': {1}", path.string(), e.what());
			return false;
		}

		const auto rootNode = data["AssetRegistry"];
		if (!rootNode)
		{
			PRS_CORE_ERROR("Invalid asset registry format");
			return false;
		}

		for (const auto& item : rootNode)
		{
			AssetHandle handle = item["Handle"].as<uint64_t>();
			AssetMetadata& metadata = m_AssetRegistry[handle];
			metadata.FilePath = item["FilePath"].as<std::string>();
			metadata.Type = AssetTypeFromString(item["Type"].as<std::string>());
		}

		return true;
	}

	const AssetMetadata& EditorAssetManager::GetMetadata(AssetHandle handle) const
	{
		static AssetMetadata s_NullMetadata;
		const auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end())
		{
			PRS_CORE_ERROR("Asset handle {0} is not valid", handle);
			return s_NullMetadata;
		}

		return it->second;
	}
}
