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

		// Wait for GPU to finish all tasks.
		static void WaitForGPU();

		static void BeginFrame();
		static void EndFrame();

		//static void BeginScene(std::shared_ptr<Texture2D> targetTexture);
		static void BeginScene();
		static void EndScene();

		static void DrawTestTriangle();
		static void DrawGUI();

		static void ProcessTestCompute();

		//static void SubmitMesh();
		//static void DrawSubmitted();
	};
}