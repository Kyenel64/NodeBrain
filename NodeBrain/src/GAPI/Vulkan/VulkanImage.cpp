#include "NBpch.h"
#include "VulkanImage.h"

namespace NodeBrain
{
	VulkanImage::VulkanImage(const ImageConfiguration& configuration)
	{
		// TODO: Create with configuration data
	}

	VulkanImage::VulkanImage(std::shared_ptr<VulkanDevice> device, VkImage image, VkFormat imageFormat)
		: m_Device(device), m_VkImage(image), m_ImageFormat(imageFormat)
	{
		NB_PROFILE_FN();

		Init();
	}

	VulkanImage::~VulkanImage()
	{
		NB_PROFILE_FN();

		if (m_VkImageView)
		{
			vkDestroyImageView(m_Device->GetVkDevice(), m_VkImageView, nullptr);
			m_VkImageView = VK_NULL_HANDLE;
		}
		
	}

	void VulkanImage::Init()
	{
		NB_PROFILE_FN();

		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_VkImage;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
		createInfo.format = m_ImageFormat;

		// TODO: parameterize
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(m_Device->GetVkDevice(), &createInfo, nullptr, &m_VkImageView));
	}
}