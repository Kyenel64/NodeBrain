#pragma once

#include "Core/Timer.h"
#include "Core/Layer.h"
#include "Core/Window.h"
#include "Core/Event.h"
#include "Core/ImGuiLayer.h"
#include "Renderer/Renderer.h"

namespace NodeBrain
{
	class App
	{
	public:
		App(const std::string& applicationName, Window* window, Renderer* renderer, ImGuiLayer* imGuiLayer);
		~App();

		void Run();
		void PushLayer(Layer* layer);

		size_t GetLayersSize() const { return m_Layers.size(); }
		Window& GetWindow() const { return *m_Window; }
		const std::string& GetApplicationName() const { return m_ApplicationName; }

	private:
		// Events
		void OnEvent(Event& event);
		void OnWindowClose(WindowClosedEvent& e);
		void OnMinimized(WindowMinimizedEvent& e);

	private:
		std::string m_ApplicationName;
		Window* m_Window;
		Renderer* m_Renderer;
		ImGuiLayer* m_ImGuiLayer;

		bool m_Running = true;
		bool m_Minimized = false;
		std::vector<Layer*> m_Layers;

		float m_LastFrameTime = 0.0f;
		Timer m_Timer;
	};
}