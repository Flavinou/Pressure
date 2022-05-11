#include "prspch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace Pressure 
{

    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        PRS_CORE_ASSERT(windowHandle, "Window handle is null!");
    }

    void OpenGLContext::Init()
    {
        PRS_PROFILE_FUNCTION();

        glfwMakeContextCurrent(m_WindowHandle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        PRS_CORE_ASSERT(status, "Failed to initialize Glad!");

        PRS_CORE_INFO("OpenGL Information :");
        PRS_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
        PRS_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
        PRS_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
    }

    void OpenGLContext::SwapBuffers()
    {
        PRS_PROFILE_FUNCTION();

        glfwSwapBuffers(m_WindowHandle);
    }

}