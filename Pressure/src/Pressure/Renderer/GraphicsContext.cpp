#include "prspch.h"
#include "Pressure/Renderer/GraphicsContext.h"

#include "Pressure/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Pressure 
{

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:	PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		PRS_CORE_ASSERT(false, "Unknown RendererAPI !");
		return nullptr;
	}

}