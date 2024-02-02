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

		bool Init();
		void SetEventCallback(std::function<void(Event&)> func) { m_Data.EventCallback = func; }
		void SwapBuffers();
		void PollEvents();

		GLFWwindow* GetGLFWWindow() const { return m_Window; }

		std::vector<const char*> GetExtensions() const { return m_Extensions; }

	private:
		GLFWwindow* m_Window = nullptr;
		std::string m_WindowName;

		WindowData m_Data;

		std::vector<const char*> m_Extensions;
	};
}