#include "prspch.h"
#include "AssetImporter.h"

#include "Pressure/Asset/AssetMetadata.h"
#include "Pressure/Asset/SceneImporter.h"
#include "Pressure/Asset/TextureImporter.h"

#include <map>

namespace Pressure
{

	using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata&)>;

	namespace
	{
		std::map<AssetType, AssetImportFunction> s_Importers = {
			{ AssetType::Texture2D, TextureImporter::ImportTexture2D },
			{ AssetType::Scene, SceneImporter::ImportScene }
		};
	}

	Ref<Asset> AssetImporter::Import(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_Importers.find(metadata.Type) != s_Importers.end())
		{
			return s_Importers[metadata.Type](handle, metadata);
		}

		PRS_CORE_ERROR("No importer registered for asset type {0}", AssetTypeToString(metadata.Type));
		return nullptr;
	}

}
