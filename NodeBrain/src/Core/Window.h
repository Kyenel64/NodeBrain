#pragma once

#include <glm/glm.hpp>

#include "Core/Event.h"

struct GLFWwindow;

namespace NodeBrain
{
	struct WindowData
	{
		int Width = 1280;
		int Height = 720;
		std::function<void(Event&)> EventCallback = nullptr;
	};

	class Window
	{
	public:
		Window(const std::string& windowName = "NodeBrain");
		~Window();

		void SetEventCallback(std::function<void(Event&)> func) { m_Data.EventCallback = func; }
		void PollEvents();

		GLFWwindow* GetGLFWWindow() const { return m_Window; }
		std::vector<const char*> GetVulkanExtensions() const { return m_VulkanExtensions; }
		glm::vec2 GetFramebufferSize() const;
		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }

	private:
		void RegisterCallbacks();

	private:
		GLFWwindow* m_Window = nullptr;
		std::string m_WindowName;

		WindowData m_Data;

		std::vector<const char*> m_VulkanExtensions;
	};
}