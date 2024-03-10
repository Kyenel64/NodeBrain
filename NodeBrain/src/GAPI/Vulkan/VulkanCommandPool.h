#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanDevice.h"

namespace NodeBrain
{
	class VulkanCommandPool
	{
	public:
		VulkanCommandPool();
		~VulkanCommandPool();

		VkCommandPool GetVkCommandPool() const { return m_VkCommandPool; }

	private:
		void Init();

	private:
		VkCommandPool m_VkCommandPool = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device;
	};
}