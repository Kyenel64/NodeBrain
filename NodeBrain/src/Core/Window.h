#pragma once

#include <glm/glm.hpp>

#include "Core/Event.h"

struct GLFWwindow;

namespace NodeBrain
{
	struct WindowData
	{
		int Width = 1920;
		int Height = 1080;
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

	private:
		GLFWwindow* m_Window = nullptr;
		std::string m_WindowName;

		WindowData m_Data;
	};
}