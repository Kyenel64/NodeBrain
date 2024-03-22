#pragma once

#include "Renderer/Framebuffer.h"

namespace NodeBrain
{
	enum class GAPI { None = 0, Vulkan };

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static GAPI GetGAPI();

		static void WaitForGPU();

		// Ran at the beginning of each frame / loop. Manages swapchain events.
		static void BeginFrame();
		static void EndFrame();

		// Ran at the beginning of each render pass.
		static void Begin(std::shared_ptr<Framebuffer> framebuffer = nullptr);
		static void End();
		
		static void DrawTestTriangle();

		//static void SubmitStaticMesh();
		//static void DrawSubmitted();

	};
}