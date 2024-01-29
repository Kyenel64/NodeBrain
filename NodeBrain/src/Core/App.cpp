#include "NBpch.h"
#include "App.h"

namespace NodeBrain
{
	App::App()
	{
		m_Timer.StartTimer();
	}

	App::~App()
	{
		for (Layer* layer : m_Layers)
			layer->OnDetach();

		m_Timer.EndTimer();
	}

	void App::Run()
	{
		while (true)
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
		}
		
	}

	void App::PushLayer(Layer* layer)
	{
		m_Layers.push_back(layer);
		layer->OnAttach();
	}
}