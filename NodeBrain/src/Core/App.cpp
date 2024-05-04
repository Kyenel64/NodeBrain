#include "NBpch.h"
#include "App.h"

#include "Core/Log.h"
#include "Core/Input.h"
#include "Renderer/Renderer.h"

namespace NodeBrain
{
	App* s_Instance = nullptr;

	App::App(const std::string& applicationName)
		: m_ApplicationName(applicationName)
	{
		NB_PROFILE_FN();

		Log::Init(); // Init first so we can utilize logging during subsystem startup.

		NB_ASSERT(!s_Instance, "Instance already exists");

		s_Instance = this;
		m_Timer.StartTimer();

		StartupSubSystems();
	}

	App::~App()
	{
		NB_PROFILE_FN();

		Renderer::WaitForGPU();

		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}

		Renderer::Shutdown();

		m_Timer.EndTimer();

		NB_INFO("Shutdown Application");
	}

	bool App::StartupSubSystems()
	{
		NB_PROFILE_FN();

		m_Window = std::make_unique<Window>("NodeBrain");
		m_Window->SetEventCallback(std::bind(&App::OnEvent, this, std::placeholders::_1));

		Renderer::Init();

		m_ImGuiLayer = ImGuiLayer::Create();
		PushLayer(m_ImGuiLayer);

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

			m_Window->AcquireNextImage();

			Renderer::BeginFrame();

			// Update
			for (Layer* layer : m_Layers)
				layer->OnUpdate(deltaTime);

			// Update GUI
			m_ImGuiLayer->BeginFrame();
			for (Layer* layer : m_Layers)
				layer->OnUpdateGUI();
			m_ImGuiLayer->EndFrame();

			Renderer::EndFrame();

			// Process input polling
			Input::ProcessStates();

			// Window 
			m_Window->SwapBuffers();
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

	App* App::Get() 
	{ 
		NB_PROFILE_FN();

		return s_Instance; 
	}
}