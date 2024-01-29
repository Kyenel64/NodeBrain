#pragma once

#include "Core/Timer.h"
#include "Core/Layer.h"
#include "Core/Window.h"

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

	private:
		std::vector<Layer*> m_Layers;
		std::unique_ptr<Window> m_Window;
		float m_LastFrameTime = 0.0f;
		Timer m_Timer;
	};
}