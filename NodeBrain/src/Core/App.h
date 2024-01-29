#pragma once

#include "Core/Timer.h"
#include "Core/Layer.h"

namespace NodeBrain
{
	class App
	{
	public:
		App();
		~App();

		void Run();
		void PushLayer(Layer* layer);

	private:
		std::vector<Layer*> m_Layers;
		float m_LastFrameTime = 0.0f;
		Timer m_Timer;
	};
}