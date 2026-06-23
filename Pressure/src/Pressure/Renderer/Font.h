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

		const MSDFData& GetMSDFData() const { return *m_Data; }
		Ref<Texture2D> GetAtlasTexture() const { return m_AtlasTexture; }

		static Ref<Font> GetDefault();

	private:
		Scope<MSDFData> m_Data;
		Ref<Texture2D> m_AtlasTexture;
	};

}
