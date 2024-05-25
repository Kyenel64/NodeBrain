#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "Renderer/RendererAPI.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Shader.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI(VulkanRenderContext* renderContext);
		virtual ~VulkanRendererAPI();

		virtual RenderContext* GetContext() const override { return m_Context; }

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual void ClearColor(const glm::vec4& color, std::shared_ptr<Image> image = nullptr) override;

		virtual void BeginRenderPass(std::shared_ptr<GraphicsPipeline> pipeline) override;
		virtual void EndRenderPass(std::shared_ptr<GraphicsPipeline> pipeline) override;
		virtual void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, uint32_t instanceCount = 1, uint32_t instanceIndex = 0) override;
		virtual void DrawIndexed(std::shared_ptr<IndexBuffer> indexBuffer, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount = 1, uint32_t instanceIndex = 0) override;

		virtual void BeginComputePass(std::shared_ptr<ComputePipeline> pipeline) override;
		virtual void EndComputePass(std::shared_ptr<ComputePipeline> pipeline) override;
		virtual void DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;

	private:
		VulkanRenderContext* m_Context;
		VulkanSwapchain& m_Swapchain;
		VkCommandBuffer m_ActiveCmdBuffer = VK_NULL_HANDLE;
		VkImage m_ActiveSwapchainImage = VK_NULL_HANDLE;
		VkImage m_DrawImage = VK_NULL_HANDLE;

		// Needed for compatibility with Vulkan 1.2
		PFN_vkCmdBeginRenderingKHR m_vkCmdBeginRenderingKHR = VK_NULL_HANDLE;
		PFN_vkCmdEndRenderingKHR m_vkCmdEndRenderingKHR = VK_NULL_HANDLE;
	};
}