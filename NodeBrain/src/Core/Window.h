#pragma once

#include <glm/glm.hpp>

#include "Core/Event.h"
#include "Renderer/RenderContext.h"

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
		RenderContext& GetRenderContext() const { return *m_RenderContext; }
		std::vector<const char*> GetExtensions() const { return m_Extensions; }
		glm::vec2 GetFramebufferSize() const;

	private:
		void RegisterCallbacks();

	private:
		GLFWwindow* m_Window = nullptr;
		std::string m_WindowName;
		std::unique_ptr<RenderContext> m_RenderContext;

		WindowData m_Data;

		std::vector<const char*> m_Extensions;
	};
}