#pragma once

#include "Renderer/DescriptorSet.h"
#include "Renderer/Image.h"
#include "GAPI/Vulkan/VulkanShader.h"


#define VK_CHECK(result) if (result) NB_ERROR(result);

namespace NodeBrain::Utils
{
	VkFormat ImageFormatToVkFormat(ImageFormat format);

	VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask);

	void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

	void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImage dstImage, VkExtent2D srcExtent, VkExtent2D dstExtent);

	VkDescriptorType BindingTypeToVkDescriptorType(BindingType type);

	VkShaderStageFlags ShaderTypeToVkShaderStageFlags(ShaderType type);
}
