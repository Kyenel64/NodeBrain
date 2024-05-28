#pragma once

#include <glm/glm.hpp>

#include "Core/Event.h"

struct GLFWwindow;

namespace NodeBrain
{
	struct WindowData
	{
		int Width;
		int Height;
		std::function<void(Event&)> EventCallback = nullptr;
	};

	class Window
	{
	public:
		Window(const std::string& windowName, uint32_t width, uint32_t height);
		~Window();

		void SetEventCallback(std::function<void(Event&)> func) { m_Data.EventCallback = func; }
		void PollEvents();

		GLFWwindow* GetGLFWWindow() const { return m_Window; }
		std::vector<const char*> GetVulkanExtensions() const { return m_VulkanExtensions; }
		uint32_t GetWidth() const { return m_Data.Width; }
		uint32_t GetHeight() const { return m_Data.Height; }
		float GetTime() const;

	private:
		void RegisterCallbacks();

	private:
		GLFWwindow* m_Window = nullptr;
		std::string m_WindowName;

		WindowData m_Data;

		std::vector<const char*> m_VulkanExtensions;
	};
}