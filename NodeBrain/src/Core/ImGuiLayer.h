#pragma once

#include "Core/Layer.h"

namespace NodeBrain
{
	class ImGuiLayer : public Layer
	{
	public:
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		static ImGuiLayer* Create();
	};
}