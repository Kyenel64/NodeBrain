#pragma once

#include <glm/glm.hpp>
#include <ImGui/imgui.h>

#include "Renderer/GraphicsPipeline.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/ComputePipeline.h"
#include "Renderer/Image.h"

namespace NodeBrain
{
	enum class GAPI { None = 0, Vulkan };

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void BeginScene(std::shared_ptr<Image> targetImage = nullptr);
		static void EndScene();

		static void RenderSubmitted();

		static void SubmitQuad(const glm::mat4& transform, const glm::vec4& color);


		// --- Backend ---
		static GAPI GetGAPI();
		static void WaitForGPU();

		static void ClearColor(const glm::vec4& color, std::shared_ptr<Image> image = nullptr);

		static void BeginRenderPass(std::shared_ptr<GraphicsPipeline> pipeline);
		static void EndRenderPass(std::shared_ptr<GraphicsPipeline> pipeline);
		static void Draw(uint32_t vertexCount, uint32_t vertexIndex, uint32_t instanceCount = 0, uint32_t instanceIndex = 0);
		static void DrawIndexed(std::shared_ptr<IndexBuffer> indexBuffer, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount = 1, uint32_t instanceIndex = 0);

		static void BeginComputePass(std::shared_ptr<ComputePipeline> pipeline);
		static void EndComputePass(std::shared_ptr<ComputePipeline> pipeline);
		static void DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ);

		static std::shared_ptr<Image> GetSwapchainDrawImage();

		// Temp
		static void ProcessGradientCompute();
	};
}