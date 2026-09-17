#pragma once

#include "Pressure/Asset/Asset.h"
#include "Pressure/Asset/AssetMetadata.h"
#include "Pressure/Renderer/Texture.h"

namespace Pressure
{

	class TextureImporter
	{
	public:
		// `AssetMetadata` file path is relative to project asset directory
		static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);

		// Reads file directly from file system, path has to be relative to working directory or absolute
		static Ref<Texture2D> LoadTexture2D(const std::filesystem::path& path);
	};

}
