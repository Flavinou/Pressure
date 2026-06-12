#pragma once

#include <filesystem>

namespace Pressure
{
	class Texture2D;
	struct MSDFData;
	
	class Font
	{
	public:
		Font(const std::filesystem::path& fontPath);
		virtual ~Font();

		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

	private:
		Scope<MSDFData> m_Data;
		Ref<Texture2D> m_AtlasTexture;
	};

}
