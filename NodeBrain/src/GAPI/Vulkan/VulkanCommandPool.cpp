#include "NBpch.h"
#include "VulkanCommandPool.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanCommandPool::VulkanCommandPool()
	{
		Init();
	}
	
	void VulkanCommandPool::Init()
	{
		NB_PROFILE_FN();

		QueueFamilyIndices queueFamilyIndices = VulkanRenderContext::GetInstance()->GetDevice()->GetPhysicalDevice()->GetQueueFamilyIndices();

		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.Graphics.value();

		VkResult result = vkCreateCommandPool(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), &commandPoolCreateInfo, nullptr, &m_VkCommandPool);
		NB_ASSERT(result == VK_SUCCESS, result);
	}

	VulkanCommandPool::~VulkanCommandPool()
	{
		if (m_VkCommandPool)
		{
			vkDestroyCommandPool(VulkanRenderContext::GetInstance()->GetDevice()->GetVkDevice(), m_VkCommandPool, nullptr);
			m_VkCommandPool = VK_NULL_HANDLE;
		}
	}
}