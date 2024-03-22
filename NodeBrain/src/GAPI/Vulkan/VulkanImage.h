#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/Image.h"
#include "GAPI/Vulkan/VulkanDevice.h"

namespace NodeBrain
{
	class VulkanImage : public Image
	{
	public:
		VulkanImage(const ImageConfiguration& configuration);
		VulkanImage(std::shared_ptr<VulkanDevice> device, VkImage image, VkFormat imageFormat);
		virtual ~VulkanImage();

		VkImageView GetVkImageView() const { return m_VkImageView; }

	private:
		void Init();

	private:
		std::shared_ptr<VulkanDevice> m_Device;

		VkImage m_VkImage = VK_NULL_HANDLE;
		VkImageView m_VkImageView = VK_NULL_HANDLE;
		VkFormat m_ImageFormat;
	};
}