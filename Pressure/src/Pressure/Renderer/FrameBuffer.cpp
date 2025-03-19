#include <prspch.h>
#include "FrameBuffer.h"

#include "Pressure/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFrameBuffer.h"

namespace Pressure
{

    Pressure::Ref<Pressure::FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:	PRS_CORE_ASSERT(false, "RendererAPI::None is currently not supported !"); return nullptr;
        case RendererAPI::API::OpenGL:  return CreateScope<OpenGLFrameBuffer>(spec);
        }

        PRS_CORE_ASSERT(false, "Unknown RendererAPI !");
        return nullptr;
    }

}