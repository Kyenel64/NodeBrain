#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Renderer/Image.h"

namespace NodeBrain
{
	class VulkanImage : public Image
	{
	public:
		VulkanImage(const ImageConfiguration& configuration);
		virtual ~VulkanImage();

		VkImage GetVkImage() const { return m_VkImage; }
		VkImageView GetVkImageView() const { return m_VkImageView; }

		const ImageConfiguration& GetConfiguration() const { return m_Configuration; }

	private:
		VkImage m_VkImage = VK_NULL_HANDLE;
		VkImageView m_VkImageView = VK_NULL_HANDLE;
		VmaAllocation m_VmaAllocation = VK_NULL_HANDLE;

		ImageConfiguration m_Configuration;
	};
}