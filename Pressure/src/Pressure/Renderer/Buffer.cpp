#include "prspch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Pressure
{

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		}

		PRS_CORE_ASSERT(false, "Unknown RendererAPI !");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* vertices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLIndexBuffer>(vertices, count);
		}

		PRS_CORE_ASSERT(false, "Unknown RendererAPI !");
		return nullptr;
	}

}