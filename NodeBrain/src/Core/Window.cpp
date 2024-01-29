#include "NBpch.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace NodeBrain
{
    static void GLFWErrorCallback(int error, const char* description)
	{
		std::cout << "GLFW Error" << error << ": " << description << std::endl;
	}

    Window::Window(const std::string& windowName)
        : m_WindowName(windowName)
    {
        Init();
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    bool Window::Init()
    {
        if (!glfwInit())
            return false;

        glfwSetErrorCallback(GLFWErrorCallback);
        
        m_Window = glfwCreateWindow(m_WindowSize.x, m_WindowSize.y, m_WindowName.c_str(), NULL, NULL);
        
        if (!m_Window)
        {
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_Window);

        // TODO: Setup window callbacks

        return true;
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }
}