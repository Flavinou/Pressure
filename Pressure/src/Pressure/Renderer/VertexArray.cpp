#include "prspch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Pressure
{

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
			case RendererAPI::API::OpenGL: return std::make_shared<OpenGLVertexArray>();
		}

		PRS_CORE_ASSERT(false, "Unknown RendererAPI !");
		return nullptr;
	}

}