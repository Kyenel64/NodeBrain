#include "NBpch.h"
#include "VulkanImage.h"

#define VMA_IMPLEMENTATION
#include <VMA/vk_mem_alloc.h>

#include "VulkanRenderContext.h"

namespace NodeBrain
{
	namespace Utils
	{
		VkFormat ImageFormatToVulkanFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::None: return VK_FORMAT_UNDEFINED; break;
				case ImageFormat::RGBA16: return VK_FORMAT_R16G16B16A16_SFLOAT; break;
			}

			return VK_FORMAT_UNDEFINED;
		}
	}
	

	VulkanImage::VulkanImage(const ImageConfiguration& configuration)
		: m_Configuration(configuration)
	{
		Init();
	}

	VulkanImage::~VulkanImage()
	{
		NB_PROFILE_FN();

		if (m_VkImageView)
		{
			vkDestroyImageView(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), m_VkImageView, nullptr);
			m_VkImageView = VK_NULL_HANDLE;
		}
		
	}

	void VulkanImage::Init()
	{
		NB_PROFILE_FN();

		// --- Create Image ---
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = Utils::ImageFormatToVulkanFormat(m_Configuration.ImageFormat);
		imageCreateInfo.extent = { m_Configuration.Width, m_Configuration.Height };

		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

		// temp settings
		VkImageUsageFlags usage = {};
		usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		usage |= VK_IMAGE_USAGE_STORAGE_BIT;
		usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		imageCreateInfo.usage = usage;


		// --- Create Image View ---
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = m_VkImage;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
		imageViewCreateInfo.format = Utils::ImageFormatToVulkanFormat(m_Configuration.ImageFormat);

		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;


		// --- Create Allocation ---
		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocationCreateInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


		// --- Create Image and View ---
		VK_CHECK(vmaCreateImage(VulkanRenderContext::Get()->GetAllocator().GetVMAAllocator(), &imageCreateInfo, &allocationCreateInfo, &m_VkImage, &m_VMAAllocation, nullptr));
		VK_CHECK(vkCreateImageView(VulkanRenderContext::Get()->GetDevice()->GetVkDevice(), &imageViewCreateInfo, nullptr, &m_VkImageView));

		// TODO: add to deletion queue
	}
}