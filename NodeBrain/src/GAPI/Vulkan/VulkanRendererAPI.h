#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/RendererAPI.h"
#include "GAPI/Vulkan/VulkanSwapChain.h"

namespace NodeBrain
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI();
		virtual ~VulkanRendererAPI();

		virtual void BeginFrame(std::shared_ptr<CommandBuffer> commandBuffer) override;
		virtual void EndFrame(std::shared_ptr<CommandBuffer> commandBuffer) override;

		// Use swapchain renderpass if renderPass == nullptr
		virtual void BeginRenderPass(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<RenderPass> renderPass = nullptr) override;
		virtual void EndRenderPass(std::shared_ptr<CommandBuffer> commandBuffer) override;
		
		virtual void DrawTestTriangle(std::shared_ptr<CommandBuffer> commandBuffer, std::shared_ptr<GraphicsPipeline> pipeline) override;

		virtual void WaitForGPU() override;

	private:
		void Init();
	};
}