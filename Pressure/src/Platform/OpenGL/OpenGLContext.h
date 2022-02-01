#pragma once

#include "Pressure/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Pressure 
{

    class OpenGLContext : public GraphicsContext
    {
    public:
        OpenGLContext(GLFWwindow* windowHandle);

        virtual void Init() override;
        virtual void SwapBuffers() override;
    private:
        GLFWwindow* m_WindowHandle;
    };

}