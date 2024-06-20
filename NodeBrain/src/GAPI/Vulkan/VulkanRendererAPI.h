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
		explicit VulkanRendererAPI(VulkanRenderContext& renderContext);
		~VulkanRendererAPI() override;

		RenderContext& GetContext() const override { return m_Context; }

		void BeginFrame() override;
		void EndFrame() override;

		void ClearColor(const glm::vec4& color, const std::shared_ptr<Image>& image) override;

		void BeginRenderPass(const std::shared_ptr<GraphicsPipeline>& pipeline) override;
		void EndRenderPass(const std::shared_ptr<GraphicsPipeline>& pipeline) override;
		void Draw(uint32_t vertexCount, uint32_t firstVertex, uint32_t instanceCount, uint32_t instanceIndex) override;
		void DrawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t instanceIndex) override;

		void BeginComputePass(const std::shared_ptr<ComputePipeline>& pipeline) override;
		void EndComputePass(const std::shared_ptr<ComputePipeline>& pipeline) override;
		void DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;

	private:
		VulkanRenderContext& m_Context;
		VulkanSwapchain& m_Swapchain;
		VkCommandBuffer m_ActiveCmdBuffer = VK_NULL_HANDLE;
		VkImage m_ActiveSwapchainImage = VK_NULL_HANDLE;
		VkImage m_DrawImage = VK_NULL_HANDLE;

		// Needed for compatibility with Vulkan 1.2
		PFN_vkCmdBeginRenderingKHR m_vkCmdBeginRenderingKHR = VK_NULL_HANDLE;
		PFN_vkCmdEndRenderingKHR m_vkCmdEndRenderingKHR = VK_NULL_HANDLE;
	};
}