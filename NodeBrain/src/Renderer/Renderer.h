#pragma once

#include "Renderer/ComputePipeline.h"
#include "Renderer/Shader.h"

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
		//static void SubmitMesh();

		static void DrawTestTriangle();
		static void DrawGUI();

		static void ProcessTestCompute();

		// Back End Functions. 
		static void BeginComputePass(std::shared_ptr<ComputePipeline> pipeline);
		static void EndComputePass();
		static void DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ);

		// Temp
		static void TempUpdateImage(std::shared_ptr<Shader> shader);

	};
}