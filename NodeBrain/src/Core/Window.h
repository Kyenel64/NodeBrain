#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

namespace NodeBrain
{
    class Window
    {
    public:
		Window(const std::string& windowName = "NodeBrain");
		~Window();

		bool Init();
		void SwapBuffers();
		void PollEvents();

    private:
		GLFWwindow* m_Window = nullptr;
		glm::vec2 m_WindowSize = { 1920.0f, 1080.0f };
		std::string m_WindowName;
    };
}