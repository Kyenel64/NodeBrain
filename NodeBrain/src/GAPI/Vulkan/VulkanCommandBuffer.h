#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/CommandBuffer.h"
#include "GAPI/Vulkan/VulkanCommandPool.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanRenderPass.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"

namespace NodeBrain
{
	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(std::shared_ptr<VulkanCommandPool> commandPool);
		virtual ~VulkanCommandBuffer();

		void Begin();
		void End();
		void StartRenderPass(std::shared_ptr<VulkanRenderPass> renderPass);
		void EndRenderPass();
		void BindGraphicsPipeline(std::shared_ptr<VulkanGraphicsPipeline> pipeline);
		void Draw();
		void Submit();


		VkCommandBuffer GetVkCommandBuffer() const { return m_VkCommandBuffer; }

	private:
		void Init();

	private:
		VkCommandBuffer m_VkCommandBuffer = VK_NULL_HANDLE;
		std::shared_ptr<VulkanCommandPool> m_CommandPool;
	};
}