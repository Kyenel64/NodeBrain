#include "NBpch.h"
#include "VulkanImage.h"

namespace NodeBrain
{
    VulkanImage::VulkanImage(std::shared_ptr<VulkanDevice> device, VkImage image, VkFormat imageFormat)
        : m_Device(device), m_Image(image)
    {
        
    }

	VulkanImage::~VulkanImage()
	{
		vkDestroyImageView(m_Device->GetVkDevice(), m_ImageView, nullptr);
	}

    void VulkanImage::Init()
    {
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_Image;
		
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
		createInfo.format = m_Format;

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

		VkResult result = vkCreateImageView(m_Device->GetVkDevice(), &createInfo, nullptr, &m_ImageView);
		NB_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image view");
    }
}