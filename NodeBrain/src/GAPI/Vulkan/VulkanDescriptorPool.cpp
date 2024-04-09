#include "NBpch.h"
#include "VulkanDescriptorPool.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanDescriptorPool::VulkanDescriptorPool(uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
	{	
		std::vector<VkDescriptorPoolSize> poolSizes;
        for (PoolSizeRatio ratio : poolRatios)
            poolSizes.push_back(VkDescriptorPoolSize{ .type = ratio.type, .descriptorCount = uint32_t(ratio.ratio * maxSets)});

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.flags = 0;
		descriptorPoolCreateInfo.maxSets = maxSets;
        descriptorPoolCreateInfo.poolSizeCount = (uint32_t)poolSizes.size();
        descriptorPoolCreateInfo.pPoolSizes = &poolSizes[0];

        VK_CHECK(vkCreateDescriptorPool(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &descriptorPoolCreateInfo, nullptr, &m_VkDescriptorPool));
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		if (m_VkDescriptorPool)
		{
			vkDestroyDescriptorPool(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_VkDescriptorPool, nullptr);
		}
	}

	void VulkanDescriptorPool::Reset()
	{
		vkResetDescriptorPool(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_VkDescriptorPool, 0);
	}
}