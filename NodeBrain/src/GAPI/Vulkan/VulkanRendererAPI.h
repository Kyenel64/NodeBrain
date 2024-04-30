#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include "Renderer/RendererAPI.h"
#include "GAPI/Vulkan/VulkanImage.h"
#include "GAPI/Vulkan/VulkanSwapchain.h"
#include "GAPI/Vulkan/VulkanIndexBuffer.h"

namespace NodeBrain
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI();
		virtual ~VulkanRendererAPI();

		virtual void WaitForGPU() override;
		virtual void ClearColor(const glm::vec4& color) override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual void BeginRenderPass() override;
		virtual void EndRenderPass() override;
		virtual void BindGraphicsPipeline(std::shared_ptr<GraphicsPipeline> pipeline) override;
		virtual void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, uint32_t instanceCount = 1, uint32_t instanceIndex = 0) override;
		virtual void DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount = 1, uint32_t instanceIndex = 0) override;

		virtual void BindIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer) override;

		virtual void BeginComputePass() override;
		virtual void EndComputePass() override;
		virtual void BindComputePipeline(std::shared_ptr<ComputePipeline> pipeline) override;
		virtual void DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;

		// Temp
		virtual void TempUpdateImage(std::shared_ptr<Shader> shader) override;

	private:
		VulkanSwapchain& m_Swapchain;
		VkCommandBuffer m_ActiveCmdBuffer = VK_NULL_HANDLE;
		VkImage m_ActiveSwapchainImage = VK_NULL_HANDLE;
		VkImage m_DrawImage = VK_NULL_HANDLE;

		// Needed for compatibility with Vulkan 1.2
		PFN_vkCmdBeginRenderingKHR m_vkCmdBeginRenderingKHR = VK_NULL_HANDLE;
		PFN_vkCmdEndRenderingKHR m_vkCmdEndRenderingKHR = VK_NULL_HANDLE;
	};
}