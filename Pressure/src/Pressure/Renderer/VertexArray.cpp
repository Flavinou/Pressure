#include "prspch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Pressure
{

	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::None: PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
			case RendererAPI::OpenGL: return new OpenGLVertexArray();
		}

		PRS_CORE_ASSERT(false, "Unknow RendererAPI !");
		return nullptr;
	}

}