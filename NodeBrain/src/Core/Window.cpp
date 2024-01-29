#include "NBpch.h"
#include "Window.h"

#include <GLFW/glfw3.h>

#include "Core/Log.h"

namespace NodeBrain
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		NB_ERROR("GLFW Error {0}: {1}", error, description);
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

		NB_INFO("Created Window {0}, size: {1}, {2}", m_WindowName, m_WindowSize.x, m_WindowSize.y);

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