#include "NBpch.h"
#include "App.h"

#include "Core/Log.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"

namespace NodeBrain
{
	App::App(const std::string& applicationName, Window* window, Renderer* renderer, ImGuiLayer* imGuiLayer)
		: m_ApplicationName(applicationName), m_Window(window), m_Renderer(renderer), m_ImGuiLayer(imGuiLayer)
	{
		NB_PROFILE_FN();

		m_Window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));

		m_Timer.StartTimer();
	}

	App::~App()
	{
		NB_PROFILE_FN();

		NB_TRACE("Test");

		for (Layer* layer : m_Layers)
			layer->OnDetach();

		m_Timer.EndTimer();

		NB_INFO("Shutdown Application");
	}

	void App::Run()
	{
		while (m_Running)
		{
			NB_PROFILE_SCOPE("Frame");

			// Calculate deltaTime
			float time = m_Timer.GetElapsedSeconds(); // TODO: Change to GLFW time
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			m_Renderer->GetContext()->AcquireNextImage();

			m_Renderer->BeginFrame();

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

			m_Renderer->EndFrame();

			Input::ProcessPollStates();

			m_Renderer->GetContext()->SwapBuffers();
			m_Window->PollEvents();
		}
	}

	void App::OnEvent(Event& event)
	{
		NB_PROFILE_FN();

		for (Layer* layer : m_Layers)
			layer->OnEvent(event);

		// Bind our functions to an event
		event.AttachEventFunction<WindowClosedEvent>(std::bind(&App::OnWindowClose, this, std::placeholders::_1));
		event.AttachEventFunction<WindowMinimizedEvent>(std::bind(&App::OnMinimized, this, std::placeholders::_1));
	}

	void App::PushLayer(Layer* layer)
	{
		NB_PROFILE_FN();

		if (!layer)
			return;

		m_Layers.push_back(layer);
		layer->OnAttach();
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