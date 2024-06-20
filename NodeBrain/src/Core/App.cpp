#include "NBpch.h"
#include "App.h"

#include "Core/Log.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"

namespace NodeBrain
{
	App::App(std::string applicationName, Window& window, Renderer& renderer, ImGuiLayer* imGuiLayer)
		: m_ApplicationName(std::move(applicationName)), m_Window(window), m_Renderer(renderer), m_ImGuiLayer(imGuiLayer)
	{
		NB_PROFILE_FN();

		m_Window.SetEventCallback([this](Event& event) { OnEvent(event); });

		m_Timer.StartTimer();
	}

	App::~App()
	{
		NB_PROFILE_FN();

		NB_INFO("Shutdown Application");
	}

	void App::Run()
	{
		while (m_Running)
		{
			NB_PROFILE_SCOPE("Frame");

			m_Window.PollEvents();

			// Calculate deltaTime
			auto time = (float)Window::GetTime();
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			m_Renderer.GetContext().AcquireNextImage();

			m_Renderer.BeginFrame();

			if (!m_Minimized)
			{
				// Update
				for (Layer* layer : m_Layers)
					layer->OnUpdate(deltaTime);

				// Update GUI
				m_ImGuiLayer->BeginFrame();
				for (Layer* layer : m_Layers)
					layer->OnUpdateGUI();
				m_ImGuiLayer->EndFrame();
			}

			m_Renderer.EndFrame();

			Input::ProcessPollStates();

			m_Renderer.GetContext().SwapBuffers();
		}
	}

	void App::OnEvent(Event& event)
	{
		NB_PROFILE_FN();

		for (Layer* layer : m_Layers)
			layer->OnEvent(event);

		// Bind our functions to an event
		event.AttachEventFunction<WindowClosedEvent>([this](WindowClosedEvent& event) { OnWindowClose(event); });
		event.AttachEventFunction<WindowMinimizedEvent>([this](WindowMinimizedEvent& event) { OnMinimized(event); });
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