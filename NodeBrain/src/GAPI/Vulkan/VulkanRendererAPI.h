#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/RendererAPI.h"

namespace NodeBrain
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI();
		virtual ~VulkanRendererAPI();

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		// Use swapchain renderpass if renderPass == nullptr
		virtual void BeginRenderPass(std::shared_ptr<RenderPass> renderPass = nullptr) override;
		virtual void EndRenderPass() override;
		
		virtual void DrawTestTriangle(std::shared_ptr<GraphicsPipeline> pipeline) override;

		virtual void WaitForGPU() override;

	private:
		void Init();
	};
}