#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanDescriptorPool.h"
#include "GAPI/Vulkan/VulkanShader.h"
#include "GAPI/Vulkan/VulkanImage.h"

namespace NodeBrain
{
	class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(std::shared_ptr<VulkanDescriptorPool> pool, std::vector<VkDescriptorSetLayoutBinding> layoutBindings, VkShaderStageFlags shaderStageFlags);
		~VulkanDescriptorSet();

		void UpdateImageData(VkImageView imageView);
		VkDescriptorSet GetVkDescriptorSet() const { return m_VkDescriptorSet; }
		VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return m_VkDescriptorSetLayout; }
		

	private:
		VkDescriptorSet m_VkDescriptorSet = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
	};
}