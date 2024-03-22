#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/CommandPool.h"
#include "GAPI/Vulkan/VulkanDevice.h"

namespace NodeBrain
{
	class VulkanCommandPool : public CommandPool
	{
	public:
		VulkanCommandPool();
		virtual ~VulkanCommandPool();

		VkCommandPool GetVkCommandPool() const { return m_VkCommandPool; }

	private:
		void Init();

	private:
		VkCommandPool m_VkCommandPool = VK_NULL_HANDLE;
	};
}