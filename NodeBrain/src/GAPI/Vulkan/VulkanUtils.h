#pragma once

#include <vulkan/vk_enum_string_helper.h>

#include "Renderer/Image.h"

namespace NodeBrain
{
	#define VK_CHECK(result) if (result) NB_ASSERT(false, string_VkResult(result))

	namespace Utils
	{
		VkFormat ImageFormatToVkFormat(ImageFormat format);
		
		VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask);

		void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

		void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImage dstImage, VkExtent2D srcExtent, VkExtent2D dstExtent);
	}
}