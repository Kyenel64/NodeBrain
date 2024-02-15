#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanDevice.h"

namespace NodeBrain
{
    class VulkanImage
    {
    public:
		VulkanImage(std::shared_ptr<VulkanDevice> device, VkImage image, VkFormat format);
		~VulkanImage();

		void Init();

    private:
		std::shared_ptr<VulkanDevice> m_Device;
		VkImage m_Image = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkFormat m_Format;
    };
}