#include "NBpch.h"
#include "Window.h"

#include <GLFW/glfw3.h>

#include "Core/KeyCode.h"

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
		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_WindowName.c_str(), NULL, NULL);
		NB_ASSERT(m_Window, "Failed to create GLFW window");
		NB_INFO("Created Window {0}, size: {1}, {2}", m_WindowName, m_Data.Width, m_Data.Height);
		// Set data when calling glfwGetWindowUserPointer
		glfwSetWindowUserPointer(m_Window, &m_Data);
		glfwMakeContextCurrent(m_Window);

		// --- Callbacks ---

		// Window events
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
			{
				WindowClosedEvent event;
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(event);
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowResizeEvent event(width, height);
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
				data.EventCallback(event);
			});
		
		// Key events
		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
			{
				if (action == GLFW_PRESS)
				{
					KeyPressedEvent event(static_cast<Key>(key));
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					data.EventCallback(event);
				}
				else if (action == GLFW_RELEASE)
				{
					KeyReleasedEvent event(static_cast<Key>(key));
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					data.EventCallback(event);
				}
			});

		// Mouse events
		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				if (action == GLFW_PRESS)
				{
					MousePressedEvent event(static_cast<MouseButton>(button));
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					data.EventCallback(event);
				}
				else if (action == GLFW_RELEASE)
				{
					MouseReleasedEvent event(static_cast<MouseButton>(button));
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					data.EventCallback(event);
				}
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) 
			{
				MouseMovedEvent event((float)xpos, (float)ypos);
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(event);
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				MouseMovedEvent event((float)xoffset, (float)yoffset);
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(event);
			});

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