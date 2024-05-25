#pragma once

namespace NodeBrain
{
	class ImGuiLayer
	{
	public:
		virtual ~ImGuiLayer() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
	};
}