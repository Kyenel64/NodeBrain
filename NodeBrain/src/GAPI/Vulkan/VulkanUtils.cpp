#include "NBpch.h"
#include "VulkanUtils.h"

#include "GAPI/Vulkan/VulkanImage.h"

namespace NodeBrain
{
	namespace Utils
	{
		VkFormat ImageFormatToVkFormat(ImageFormat format)
		{
			switch (format)
			{
				case ImageFormat::None: return VK_FORMAT_UNDEFINED; break;
				case ImageFormat::RGBA16: return VK_FORMAT_R16G16B16A16_SFLOAT; break;
				case ImageFormat::RGBA8: return VK_FORMAT_B8G8R8A8_SRGB; break;
			}

			return VK_FORMAT_UNDEFINED;
		}

		VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspectMask)
		{
			VkImageSubresourceRange subImage = {};
			subImage.aspectMask = aspectMask;
			subImage.baseMipLevel = 0;
			subImage.levelCount = VK_REMAINING_MIP_LEVELS;
			subImage.baseArrayLayer = 0;
			subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

			return subImage;
		}

		void TransitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
		{
			VkImageMemoryBarrier imageBarrier = {};
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

			imageBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
			imageBarrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
			imageBarrier.oldLayout = currentLayout;
			imageBarrier.newLayout = newLayout;

			VkImageAspectFlags aspectFlags = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			imageBarrier.subresourceRange = ImageSubresourceRange(aspectFlags);
			imageBarrier.image = image;

			vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);
		}

		void CopyImageToImage(VkCommandBuffer commandBuffer, VkImage srcImage, VkImage dstImage, VkExtent2D srcExtent, VkExtent2D dstExtent)
		{
			VkImageBlit blitRegion = {};
			blitRegion.srcOffsets[1].x = srcExtent.width;
			blitRegion.srcOffsets[1].y = srcExtent.height;
			blitRegion.srcOffsets[1].z = 1;

			blitRegion.dstOffsets[1].x = dstExtent.width;
			blitRegion.dstOffsets[1].y = dstExtent.height;
			blitRegion.dstOffsets[1].z = 1;

			blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blitRegion.srcSubresource.baseArrayLayer = 0;
			blitRegion.srcSubresource.layerCount = 1;
			blitRegion.srcSubresource.mipLevel = 0;

			blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blitRegion.dstSubresource.baseArrayLayer = 0;
			blitRegion.dstSubresource.layerCount = 1;
			blitRegion.dstSubresource.mipLevel = 0;

			vkCmdBlitImage(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blitRegion, VK_FILTER_LINEAR);
		}

		VkDescriptorType BindingTypeToVkDescriptorType(BindingType type)
		{
			switch (type)
			{
				case BindingType::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				case BindingType::UniformBuffer: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			}

			NB_ASSERT(false, "Unavailable binding type");
			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}

		VkShaderStageFlags ShaderTypeToVkShaderStageFlags(ShaderType type)
		{
			switch (type)
			{
				case ShaderType::Vertex: return VK_SHADER_STAGE_VERTEX_BIT;
				case ShaderType::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
				case ShaderType::Compute: return VK_SHADER_STAGE_COMPUTE_BIT;
			}

			NB_ASSERT(false, "Unavailable shader type");
			return 0;
		}
	}
	
}