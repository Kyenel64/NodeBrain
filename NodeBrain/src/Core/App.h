#pragma once

#include "Layer.h"

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
	};
}