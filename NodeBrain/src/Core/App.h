#pragma once

#include "Core/Timer.h"
#include "Core/Layer.h"
#include "Core/Window.h"
#include "Core/Event.h"

namespace NodeBrain
{
	class App
	{
	public:
		App();
		~App();

		void Run();
		void PushLayer(Layer* layer);

		size_t GetLayersSize() const { return m_Layers.size(); }
		Window& GetWindow() const { return *m_Window; }
		static App* GetInstance();

	private:
		// Events
		void OnEvent(Event& event);
		void OnWindowClose(WindowClosedEvent& e);

		bool StartupSubSystems();

	private:
		bool m_Running = true;
		std::vector<Layer*> m_Layers;
		std::unique_ptr<Window> m_Window;
		float m_LastFrameTime = 0.0f;
		Timer m_Timer;
	};
}