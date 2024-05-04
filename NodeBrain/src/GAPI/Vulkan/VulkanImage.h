#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Renderer/Renderer.h"
#include "Renderer/Image.h"

namespace NodeBrain
{
	class VulkanImage : public Image
	{
	public:
		VulkanImage(const ImageConfiguration& configuration);
		virtual ~VulkanImage();

		virtual uint64_t GetAddress() override;

		VkImage GetVkImage() const { return m_VkImage; }
		VkImageView GetVkImageView() const { return m_VkImageView; }

		virtual const ImageConfiguration& GetConfiguration() const override { return m_Configuration; }

	private:
		VkImage m_VkImage = VK_NULL_HANDLE;
		VkImageView m_VkImageView = VK_NULL_HANDLE;
		VmaAllocation m_VmaAllocation = VK_NULL_HANDLE;
		VkSampler m_VkSampler = VK_NULL_HANDLE;

		ImageConfiguration m_Configuration;

		uint64_t m_Address = 0;
	};
}