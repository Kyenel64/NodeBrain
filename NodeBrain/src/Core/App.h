#pragma once

#include "Core/Timer.h"
#include "Core/Window.h"
#include "Core/Event.h"
#include "Core/ImGuiLayer.h"
#include "Renderer/Renderer.h"

#define NB_BIND_EVENT_FN(fn, obj) std::bind(fn, obj, std::placeholders::_1)

namespace NodeBrain
{
	class App
	{
	public:
		App(std::string applicationName, Window& window, Renderer& renderer, ImGuiLayer* imGuiLayer);
		~App();

		void BeginFrame();
		void EndFrame();

		void AddEventFunction(const std::function<void(Event&)>& func) { m_EventFunctions.push_back(func); }

		[[nodiscard]] bool IsRunning() const { return m_Running; }
		[[nodiscard]] float GetDeltaTime() const { return m_DeltaTime; }

	private:
		// Events
		void OnEvent(Event& event);
		void OnWindowClose(WindowClosedEvent& e);
		void OnMinimized(WindowMinimizedEvent& e);

	private:
		std::string m_ApplicationName;
		Window& m_Window;
		Renderer& m_Renderer;
		ImGuiLayer* m_ImGuiLayer = nullptr;

		bool m_Running = true;
		bool m_Minimized = false;

		std::vector<std::function<void(Event&)>> m_EventFunctions;

		float m_DeltaTime = 0.0f;
		double m_LastFrameTime = 0.0f;
		Timer m_Timer;
	};
}
