#include "prspch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Pressure
{

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}