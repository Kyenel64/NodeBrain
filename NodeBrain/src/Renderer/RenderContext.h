#pragma once

#include "Renderer/RenderPass.h"

namespace NodeBrain
{
	class Window;

	class RenderContext
	{
	public:
		virtual ~RenderContext() = default;

		virtual void Init() = 0;
		virtual void AcquireNextImage() = 0;
		virtual void SwapBuffers() = 0;

		static std::unique_ptr<RenderContext> Create(Window* window);
	};
}