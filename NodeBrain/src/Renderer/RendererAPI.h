#pragma once

#include <glm/glm.hpp>

#include "Renderer/GraphicsPipeline.h"
#include "Renderer/ComputePipeline.h"
#include "Renderer/Framebuffer.h"

namespace NodeBrain
{
	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void WaitForGPU() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void BeginRenderPass(std::shared_ptr<RenderPass> renderPass = nullptr) = 0;
		virtual void EndRenderPass() = 0;

		virtual void BeginComputePass(std::shared_ptr<ComputePipeline> pipeline) = 0;
		virtual void EndComputePass() = 0;
		virtual void DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) = 0;

		virtual void ClearColor(const glm::vec4& color) = 0;

		virtual void DrawTestTriangle(std::shared_ptr<GraphicsPipeline> pipeline) = 0;
		virtual void TempUpdateImage(std::shared_ptr<Shader> shader) = 0;


		static std::unique_ptr<RendererAPI> Create();
	};
}