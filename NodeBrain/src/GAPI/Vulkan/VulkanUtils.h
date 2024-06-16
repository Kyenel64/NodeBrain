#pragma once

#include <vulkan/vulkan_to_string.hpp>

#include "Renderer/DescriptorSet.h"
#include "Renderer/Image.h"
#include "GAPI/Vulkan/VulkanShader.h"


#define VK_CHECK(result) if (result) NB_ERROR(vulkan_to_string(result));

namespace NodeBrain::Utils
{
	VkFormat ImageFormatToVkFormat(ImageFormat format);

	VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask);

	void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

	void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImage dstImage, VkExtent2D srcExtent, VkExtent2D dstExtent);

	VkDescriptorType BindingTypeToVkDescriptorType(BindingType type);

	VkShaderStageFlags ShaderTypeToVkShaderStageFlags(ShaderType type);
}
