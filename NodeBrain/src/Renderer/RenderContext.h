#pragma once

#include "Core/Window.h"

namespace NodeBrain
{
	enum class GAPI { None = 0, Vulkan };

	class RenderContext
	{
	public:
		virtual ~RenderContext() = default;

		virtual void AcquireNextImage() = 0;
		virtual void SwapBuffers() = 0;

		virtual void WaitForGPU() = 0;

		[[nodiscard]] virtual GAPI GetGraphicsAPI() const = 0;
		[[nodiscard]] virtual Window& GetWindow() const = 0;
	};
}