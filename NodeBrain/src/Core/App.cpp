#include "NBpch.h"
#include "App.h"

#include "Core/Log.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"

namespace NodeBrain
{
	static bool firstFrame = true;

	App::App(std::string applicationName, Window& window, Renderer& renderer, ImGuiLayer* imGuiLayer)
		: m_ApplicationName(std::move(applicationName)), m_Window(window), m_Renderer(renderer), m_ImGuiLayer(imGuiLayer)
	{
		NB_PROFILE_FN();

		m_Window.SetEventCallback([this](Event& event) { OnEvent(event); });
	}

	App::~App()
	{
		NB_PROFILE_FN();

		NB_INFO("Shutdown Application");
	}

	void App::BeginFrame()
	{
		// Mainly to switch buffers from updating both frames during startup to current frame during runtime.
		if (firstFrame)
		{
			m_Renderer.GetContext().OnFirstFrame();
			firstFrame = false;
		}

		m_Window.PollEvents();

		// Calculate deltaTime
		double time = m_Timer.GetElapsedTime(TimerUnit::Seconds);
		m_DeltaTime = static_cast<float>(time - m_LastFrameTime);
		m_LastFrameTime = time;

		m_Renderer.GetContext().AcquireNextImage();
		m_Renderer.BeginFrame();

		m_ImGuiLayer->BeginFrame();
	}

	void App::EndFrame()
	{
		m_ImGuiLayer->EndFrame();

		m_Renderer.EndFrame();

		Input::ProcessPollStates();

		m_Renderer.GetContext().SwapBuffers();
	}

	void App::OnEvent(Event& event)
	{
		NB_PROFILE_FN();

		for (auto& func : m_EventFunctions)
			func(event);

		// Bind our functions to an event
		event.AttachEventFunction<WindowClosedEvent>(NB_BIND_EVENT_FN(&App::OnWindowClose, this));
		event.AttachEventFunction<WindowMinimizedEvent>(NB_BIND_EVENT_FN(&App::OnMinimized, this));
	}

	void App::OnWindowClose(WindowClosedEvent& e)
	{
		NB_PROFILE_FN();

		m_Running = false;
	}

	void App::OnMinimized(WindowMinimizedEvent& e)
	{
		NB_PROFILE_FN();

		m_Minimized = e.IsMinimized();
	}
}
