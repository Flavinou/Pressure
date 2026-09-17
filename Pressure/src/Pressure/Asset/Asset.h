#pragma once

#include "Pressure/Core/UUID.h"

namespace Pressure
{
	using AssetHandle = UUID;

	enum class AssetType : uint8_t
	{
		None = 0,
		Scene,
		Texture2D
	};

	inline std::string_view AssetTypeToString(AssetType e)
	{
		switch (e)
		{
			case AssetType::None: return "AssetType::None";
			case AssetType::Scene: return "AssetType::Scene";
			case AssetType::Texture2D: return "AssetType::Texture2D";
			default: return "AssetType::<Invalid>";
		}
	}

	inline AssetType AssetTypeFromString(std::string_view str)
	{
		if (str == "AssetType::None") return AssetType::None;
		if (str == "AssetType::Scene") return AssetType::Scene;
		if (str == "AssetType::Texture2D") return AssetType::Texture2D;

		return AssetType::None;
	}

	class Asset
	{
	public:
		AssetHandle m_Handle; // Generates a handle automatically

		virtual AssetType GetType() const = 0;
	};

}
