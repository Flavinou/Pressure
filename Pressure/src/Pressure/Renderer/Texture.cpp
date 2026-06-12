#include "prspch.h"
#include "Pressure/Renderer/Texture.h"

#include "Pressure/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Pressure
{
	Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(specification);
		}

		PRS_CORE_ASSERT(false, "Unknown RendererAPI !");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
			case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
		}

		PRS_CORE_ASSERT(false, "Unknown RendererAPI !");
		return nullptr;
	}

}
