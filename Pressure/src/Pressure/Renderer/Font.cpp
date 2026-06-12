#include "prspch.h"
#include "Font.h"

#include <msdf-atlas-gen.h>

namespace Pressure
{
	Font::Font(const std::filesystem::path& fontPath)
	{
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		if (!ft)
		{
			PRS_CORE_ERROR("Failed to initialize FreeType library");
			return;
		}

		std::string filePathStr = fontPath.string();
		msdfgen::FontHandle* font = msdfgen::loadFont(ft, filePathStr.c_str());
		if (!font)
		{
			PRS_CORE_ERROR("Failed to load font at '{}'", filePathStr);
			return;
		}

		msdfgen::Shape shape;
		if (!msdfgen::loadGlyph(shape, font, 'C'))
		{
			PRS_CORE_ERROR("Failed to load glyph for character 'C'");
			return;
		}

		shape.normalize();
		msdfgen::edgeColoringSimple(shape, 3.0);
		msdfgen::Bitmap<float, 3> msdf(32, 32);
		msdfgen::generateMSDF(msdf, shape, 4.0, 1.0, msdfgen::Vector2(4.0, 4.0));

		// Dependency to PNG library is not included in msdfgen, I'll leave it as-is while working on generating font atlases
		// msdfgen::savePng(msdf, "output.png");

		// Cleanup
		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);
	}
}
