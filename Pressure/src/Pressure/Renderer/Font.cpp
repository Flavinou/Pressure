#include "prspch.h"
#include "Font.h"

#include "Pressure/Renderer/MSDFData.h"
#include "Pressure/Renderer/Texture.h"

#include <msdf-atlas-gen.h>

namespace Pressure
{

	namespace
	{
		template <typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
		Ref<Texture2D> CreateAndCacheAtlas(const std::vector<msdf_atlas::GlyphGeometry>& glyphs
			, uint32_t width
			, uint32_t height)
		{
			msdf_atlas::GeneratorAttributes attributes;
			attributes.config.overlapSupport = true;
			attributes.scanlinePass = true;

			msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
			generator.setAttributes(attributes);
			generator.setThreadCount(8);
			generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

			msdfgen::BitmapConstRef<T, N> bitmap = static_cast<msdfgen::BitmapConstRef<T, N>>(generator.atlasStorage());

			TextureSpecification spec;
			spec.Width = bitmap.width;
			spec.Height = bitmap.height;
			spec.Format = ImageFormat::RGB8;
			spec.GenerateMips = false;
			spec.Clamp = true;

			Ref<Texture2D> texture = Texture2D::Create(spec);
			texture->SetData((void*)bitmap.pixels, bitmap.width * bitmap.height * 3);
			return texture;
		}
	}

	Font::Font(const std::filesystem::path& fontPath)
		: m_Data(CreateScope<MSDFData>())
	{
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		if (!ft)
		{
			PRS_CORE_ERROR("Failed to initialize FreeType library");
			return;
		}

		std::string filePathStr = fontPath.string();

		// TODO: msdfgen::loadFontData loads from memory buffer which we'll need 
		msdfgen::FontHandle* font = msdfgen::loadFont(ft, filePathStr.c_str());
		if (!font)
		{
			PRS_CORE_ERROR("Failed to load font at '{}'", filePathStr);
			return;
		}

		// Load glyphs and generate MSDF atlas
		struct CharsetRange
		{
			uint32_t Begin, End;
		};

		static constexpr CharsetRange charsetRanges[] =
		{
			{ 0x20, 0x7E }, // Basic Latin
			{ 0xA0, 0xFF }, // Latin-1 Supplement
		};

		msdf_atlas::Charset charset;
		for (CharsetRange range : charsetRanges)
		{
			for (uint32_t c = range.Begin; c <= range.End; c++)
			{
				charset.add(c);
			}
		}

		constexpr double fontScale = 1.0;
		m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
		int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
		PRS_CORE_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());

		double emSize = 40.0;
		msdf_atlas::TightAtlasPacker packer;
		packer.setScale(emSize);
		packer.setPixelRange(4.0);
		packer.setMiterLimit(1.0);
		packer.setOuterPixelPadding(0.5);
		const int remaining = packer.pack(m_Data->Glyphs.data(), static_cast<int>(m_Data->Glyphs.size()));
		PRS_CORE_ASSERT(remaining == 0);

		int width, height;
		packer.getDimensions(width, height);

		// MSDF || MTSDF
		for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
		{
			constexpr double maxCornerAngle = 3.0;
			glyph.edgeColoring(&msdfgen::edgeColoringByDistance, maxCornerAngle, 0);
		}

		m_AtlasTexture = CreateAndCacheAtlas<msdf_atlas::byte, float, 3, msdf_atlas::msdfGenerator>(
			m_Data->Glyphs
			, width
			, height
		);

		// Cleanup
		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);
	}

	Font::~Font()
	{
	}

	Ref<Font> Font::GetDefault()
	{
		static Ref<Font> DefaultFont;
		if (!DefaultFont)
		{
			DefaultFont = CreateRef<Font>("assets/fonts/roboto/Roboto-Regular.ttf");
		}

		return DefaultFont;
	}
}
