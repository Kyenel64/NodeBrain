#include "NBpch.h"
#include "Window.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Core/Input.h"
#include "Core/KeyCode.h"

namespace NodeBrain
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		NB_ERROR("GLFW Error {0}: {1}", error, description);
	}

	Window::Window(const std::string& windowName, uint32_t width, uint32_t height)
		: m_WindowName(windowName)
	{
		NB_PROFILE_FN();

		NB_ASSERT(width, "width is 0. Width must be a non-zero value.");
		NB_ASSERT(height, "height is 0. Height must be a non-zero value.");

		m_Data.Width = width;
		m_Data.Height = height;

		if (!glfwInit())
			NB_ASSERT(false, "Failed to initialize GLFW.");

		glfwSetErrorCallback(GLFWErrorCallback);
		
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		#ifdef NB_APPLE
			glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
		#endif

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_WindowName.c_str(), NULL, NULL);
		NB_ASSERT(m_Window, "Failed to create GLFW window");
		NB_INFO("Created Window {0}, size: {1}, {2}", m_WindowName, m_Data.Width, m_Data.Height);
		// Set data that can be accessed when calling glfwGetWindowUserPointer
		glfwSetWindowUserPointer(m_Window, &m_Data);

		// Vulkan extensions
		uint32_t extensionCount = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
		for (size_t i = 0; i < extensionCount; i++)
			m_VulkanExtensions.push_back(extensions[i]);
		
		RegisterCallbacks();
	}

	Window::~Window()
	{
		NB_PROFILE_FN();

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::PollEvents()
	{
		NB_PROFILE_FN();

		glfwPollEvents();
	}

	void Window::RegisterCallbacks()
	{
		NB_PROFILE_FN();

		// Window events
		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
			{
				WindowClosedEvent event;
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.EventCallback(event);
			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowResizedEvent event(width, height);
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
				data.EventCallback(event);
			});

		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
		{
			WindowMinimizedEvent event(iconified);
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
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
					Input::SetKeyState(static_cast<Key>(key), InputState::Pressed);
				}
				else if (action == GLFW_RELEASE)
				{
					KeyReleasedEvent event(static_cast<Key>(key));
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					data.EventCallback(event);
					Input::SetKeyState(static_cast<Key>(key), InputState::Released);
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
					Input::SetMouseButtonState(static_cast<MouseButton>(button), InputState::Pressed);
				}
				else if (action == GLFW_RELEASE)
				{
					MouseReleasedEvent event(static_cast<MouseButton>(button));
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					data.EventCallback(event);
					Input::SetMouseButtonState(static_cast<MouseButton>(button), InputState::Released);
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
	}
}