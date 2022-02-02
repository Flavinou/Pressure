#include "prspch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Pressure
{

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
			case RendererAPI::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}

		PRS_CORE_ASSERT(false, "Unknow RendererAPI !");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* vertices, uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
			case RendererAPI::OpenGL: return new OpenGLIndexBuffer(vertices, count);
		}

		PRS_CORE_ASSERT(false, "Unknow RendererAPI !");
		return nullptr;
	}

}