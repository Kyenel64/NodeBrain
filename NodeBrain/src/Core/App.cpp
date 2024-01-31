#include "NBpch.h"
#include "App.h"

#include "Core/Log.h"

namespace NodeBrain
{
	App::App()
	{
		m_Timer.StartTimer();

		Log::Init();

		m_Window = std::make_unique<Window>("NodeBrain");
		m_Window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));
	}

	App::~App()
	{
		for (Layer* layer : m_Layers)
			layer->OnDetach();

		m_Timer.EndTimer();
	}

	void App::Run()
	{
		while (m_Running)
		{
			// Calculate deltaTime
			float time = m_Timer.GetElapsedSeconds(); // TODO: Change to GLFW time
			float deltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			// Update
			for (Layer* layer : m_Layers)
				layer->OnUpdate(deltaTime);

			// Update GUI
			for (Layer* layer : m_Layers)
				layer->OnUpdateGUI();

			// Window 
			m_Window->SwapBuffers();
			m_Window->PollEvents();
		}
	}

	void App::OnEvent(Event& event)
	{
		for (Layer* layer : m_Layers)
			layer->OnEvent(event);

		// Bind our functions to an event
		event.AttachEventFunction<WindowClosedEvent>(std::bind(&App::OnWindowClose, this, std::placeholders::_1));
	}

	void App::PushLayer(Layer* layer)
	{
		if (!layer)
			return;

		m_Layers.push_back(layer);
		layer->OnAttach();
	}

	void App::OnWindowClose(WindowClosedEvent& e)
	{
		NB_INFO("Closing Application");
		m_Running = false;
	}
}