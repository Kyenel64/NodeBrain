#include "nbpch.h"
#include "App.h"

namespace NodeBrain
{
	App::App()
	{

	}

	App::~App()
	{
		for (Layer* layer : m_Layers)
			layer->OnDetach();
	}

	void App::Run()
	{
		while (true)
		{
			// Update
			for (Layer* layer : m_Layers)
				layer->OnUpdate();

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