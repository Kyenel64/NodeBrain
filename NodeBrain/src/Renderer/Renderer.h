#pragma once

#include "Renderer/GraphicsPipeline.h"
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

		static void BeginFrame();
		static void EndFrame();

		static void BeginScene();
		static void EndScene();


		// Back End Functions.
		static void WaitForGPU();

		static void BeginRenderPass();
		static void EndRenderPass();
		static void BindGraphicsPipeline(std::shared_ptr<GraphicsPipeline> pipeline);
		static void Draw(uint32_t vertexCount, uint32_t vertexIndex, uint32_t instanceCount = 0, uint32_t instanceIndex = 0);

		static void BeginComputePass();
		static void EndComputePass();
		static void BindComputePipeline(std::shared_ptr<ComputePipeline> pipeline);
		static void DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ);



		// Temp
		static void TempUpdateImage(std::shared_ptr<Shader> shader);
		static void ProcessGradientCompute();
		static void ProcessFlatColorCompute();
	};
}