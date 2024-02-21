#include "NBpch.h"
#include "App.h"

#include "Core/Log.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"

namespace NodeBrain
{
	App* s_Instance = nullptr;

	App::App()
	{
		s_Instance = this;
		m_Timer.StartTimer();

		StartupSubSystems();
	}

	App::~App()
	{
		Renderer::Shutdown();
		
		for (Layer* layer : m_Layers)
			layer->OnDetach();

		m_Timer.EndTimer();

		NB_INFO("Shutdown Application");
	}

	bool App::StartupSubSystems()
	{
		Log::Init();

		m_Window = std::make_unique<Window>("NodeBrain");
		m_Window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));

		Renderer::Init();

		return true;
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

			// Update
			for (Layer* layer : m_Layers)
				layer->OnUpdate(deltaTime);

			// Update GUI
			for (Layer* layer : m_Layers)
				layer->OnUpdateGUI();

			// Process input polling
			Input::ProcessStates();

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
		m_Running = false;
	}

	App* App::GetInstance() 
	{ 
		return s_Instance; 
	}
}