#pragma once

#include "Core/Timer.h"
#include "Core/Layer.h"
#include "Core/Window.h"
#include "Core/Event.h"
#include "Core/ImGuiLayer.h"

namespace NodeBrain
{
	class App
	{
	public:
		App(const std::string& applicationName);
		~App();

		void Run();
		void PushLayer(Layer* layer);

		static App* Get();
		size_t GetLayersSize() const { return m_Layers.size(); }
		Window& GetWindow() const { return *m_Window; }
		const std::string& GetApplicationName() const { return m_ApplicationName; }

	private:
		// Events
		void OnEvent(Event& event);
		void OnWindowClose(WindowClosedEvent& e);

		bool StartupSubSystems();

	private:
		std::string m_ApplicationName;
		bool m_Running = true;
		std::vector<Layer*> m_Layers;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;

		float m_LastFrameTime = 0.0f;
		Timer m_Timer;
	};
}