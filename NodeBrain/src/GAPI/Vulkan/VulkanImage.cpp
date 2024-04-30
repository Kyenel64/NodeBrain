#include "NBpch.h"
#include "VulkanImage.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanUtils.h"

namespace NodeBrain
{	
	VulkanImage::VulkanImage(const ImageConfiguration& configuration)
		: m_Configuration(configuration)
	{
		NB_PROFILE_FN();

		// --- Create Image ---
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = ImageFormatToVkFormat(m_Configuration.Format);
		imageCreateInfo.extent = { m_Configuration.Width, m_Configuration.Height, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

		// Usage flags
		VkImageUsageFlags usage = {};
		usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		usage |= VK_IMAGE_USAGE_STORAGE_BIT;
		usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		imageCreateInfo.usage = usage;

		// Allocation
		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocationCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VK_CHECK(vmaCreateImage(VulkanRenderContext::Get()->GetVMAAllocator(), &imageCreateInfo, &allocationCreateInfo, &m_VkImage, &m_VmaAllocation, nullptr));


		// --- Create Image View ---
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = m_VkImage;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
		imageViewCreateInfo.format = ImageFormatToVkFormat(m_Configuration.Format);

		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(VulkanRenderContext::Get()->GetVkDevice(), &imageViewCreateInfo, nullptr, &m_VkImageView));
	}

	VulkanImage::~VulkanImage()
	{
		NB_PROFILE_FN();

		vkDestroyImageView(VulkanRenderContext::Get()->GetVkDevice(), m_VkImageView, nullptr);
		m_VkImageView = VK_NULL_HANDLE;

		vmaDestroyImage(VulkanRenderContext::Get()->GetVMAAllocator(), m_VkImage, m_VmaAllocation);
		m_VkImage = VK_NULL_HANDLE;
		m_VmaAllocation = VK_NULL_HANDLE;
	}
}