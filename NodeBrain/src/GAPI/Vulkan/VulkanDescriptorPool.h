#pragma once

#include <span>

#include <vulkan/vulkan.h>

namespace NodeBrain
{
	struct PoolSizeRatio
	{
		VkDescriptorType type;
		float ratio;
	};



    class VulkanDescriptorPool
    {
    public:
		VulkanDescriptorPool(uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
		~VulkanDescriptorPool();

		VkDescriptorPool GetVkDescriptorPool() const { return m_VkDescriptorPool; }

		void Reset();
    private:
		VkDescriptorPool m_VkDescriptorPool = VK_NULL_HANDLE;
    };
}