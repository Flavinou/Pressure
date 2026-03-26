#include "prspch.h"
#include "UniformBuffer.h"

#include "Renderer.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Pressure
{

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		PRS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
