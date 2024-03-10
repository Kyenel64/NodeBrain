#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanCommandPool.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanRenderPass.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"

namespace NodeBrain
{
	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBuffer(std::shared_ptr<VulkanCommandPool> commandPool);
		~VulkanCommandBuffer();

		void Begin();
		void End();
		void StartRenderPass(std::shared_ptr<VulkanRenderPass> renderPass, std::shared_ptr<VulkanFramebuffer> framebuffers);
		void EndRenderPass();
		void Draw();

		void BindGraphicsPipeline(std::shared_ptr<VulkanGraphicsPipeline> pipeline);

		VkCommandBuffer GetVkCommandBuffer() const { return m_VkCommandBuffer; }

	private:
		void Init();

	private:
		VkCommandBuffer m_VkCommandBuffer = VK_NULL_HANDLE;
		std::shared_ptr<VulkanCommandPool> m_CommandPool;
		std::shared_ptr<VulkanDevice> m_Device;
	};
}