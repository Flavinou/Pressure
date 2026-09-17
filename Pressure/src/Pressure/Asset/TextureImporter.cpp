#include "prspch.h"
#include "TextureImporter.h"

#include "Pressure/Asset/AssetMetadata.h"
#include "Pressure/Core/Buffer.h"
#include "Pressure/Project/Project.h"
#include "Pressure/Renderer/Texture.h"

#include <stb_image.h>

namespace Pressure
{

	Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		PRS_PROFILE_FUNCTION();

		return LoadTexture2D(Project::GetAssetDirectory() / metadata.FilePath);
	}

	Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
	{
		PRS_PROFILE_FUNCTION();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		Buffer data;

		{
			PRS_PROFILE_SCOPE("stbi_load - TextureImporter::Import");
			std::string pathStr = path.string();
			data.Data = reinterpret_cast<std::byte*>(stbi_load(pathStr.c_str(), &width, &height, &channels, 0));
		}

		if (!data.Data)
		{
			PRS_CORE_ERROR("Failed to load texture from path: {0}", path.string());
			return nullptr;
		}

		data.Size = width * height * channels;

		TextureSpecification spec;
		spec.Width = width;
		spec.Height = height;
		switch (channels)
		{
			case 3:
				spec.Format = ImageFormat::RGB8;
				break;
			case 4:
				spec.Format = ImageFormat::RGBA8;
				break;
		}

		Ref<Texture2D> texture = Texture2D::Create(spec, data);

		data.Release();
		return texture;
	}
}
