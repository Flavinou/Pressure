#include "prspch.h"
#include "Font.h"

#include "Pressure/Renderer/Texture.h"

#include <msdf-atlas-gen.h>

namespace Pressure
{

	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
	};

	namespace
	{
		template <typename T, typename S, int N, msdf_atlas::GeneratorFunction<S, N> GenFunc>
		Ref<Texture2D> CreateAndCacheAtlas(const std::string& fontName
			, float fontSize
			, const std::vector<msdf_atlas::GlyphGeometry>& glyphs
			, const msdf_atlas::FontGeometry& fontGeometry
			, uint32_t width
			, uint32_t height)
		{
			msdf_atlas::GeneratorAttributes attributes;
			attributes.config.overlapSupport = true;
			attributes.scanlinePass = true;

			msdf_atlas::ImmediateAtlasGenerator<S, N, GenFunc, msdf_atlas::BitmapAtlasStorage<T, N>> generator(width, height);
			generator.setAttributes(attributes);
			generator.generate(glyphs.data(), static_cast<int>(glyphs.size()));

			msdfgen::BitmapConstRef<T, N> bitmap = static_cast<msdfgen::BitmapConstRef<T, N>>(generator.atlasStorage());

			TextureSpecification spec;
			spec.Width = bitmap.width;
			spec.Height = bitmap.height;
			spec.Format = ImageFormat::RGB8;
			spec.GenerateMips = false;

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
		msdf_atlas::TightAtlasPacker atlasPacker;
		atlasPacker.setPixelRange(2.0);
		atlasPacker.setMiterLimit(1.0);
		atlasPacker.setInnerPixelPadding(0.0);
		atlasPacker.setOuterPixelPadding(0.0);
		atlasPacker.setInnerUnitPadding(0.0);
		atlasPacker.setOuterUnitPadding(0.0);
		atlasPacker.setScale(emSize);
		const int remaining = atlasPacker.pack(m_Data->Glyphs.data(), static_cast<int>(m_Data->Glyphs.size()));
		PRS_CORE_ASSERT(remaining == 0);

		int width, height;
		atlasPacker.getDimensions(width, height);
		emSize = atlasPacker.getScale();

		m_AtlasTexture = CreateAndCacheAtlas<uint8_t, float, 3, msdf_atlas::msdfGenerator>(
			"Test"
			, static_cast<float>(emSize)
			, m_Data->Glyphs
			, m_Data->FontGeometry
			, width, height
		);

		// Cleanup
		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);
	}

	Font::~Font()
	{
	}
}
