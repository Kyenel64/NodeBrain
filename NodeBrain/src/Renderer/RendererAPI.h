#pragma once

#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/CommandBuffer.h"

namespace NodeBrain
{
	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void BeginFrame(std::shared_ptr<CommandBuffer> commandBuffer) = 0;
		virtual void EndFrame(std::shared_ptr<CommandBuffer> commandBuffer) = 0;

		virtual void BeginRenderPass(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<RenderPass> renderPass = nullptr) = 0;
		virtual void EndRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) = 0;

		virtual void DrawTestTriangle(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline) = 0;

		virtual void WaitForGPU() = 0;

		static std::unique_ptr<RendererAPI> Create();
	};
}