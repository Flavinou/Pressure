#pragma once

#include "Pressure/Asset/Asset.h"

namespace Pressure
{

	struct AssetMetadata;

	class AssetImporter
	{
	public:
		static Ref<Asset> Import(AssetHandle handle, const AssetMetadata& metadata);
	};

}
