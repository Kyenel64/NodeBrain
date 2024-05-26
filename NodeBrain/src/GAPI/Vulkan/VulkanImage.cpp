#include "NBpch.h"
#include "VulkanImage.h"

#include <ImGui/backends/imgui_impl_vulkan.h>

#include "GAPI/Vulkan/VulkanUtils.h"

namespace NodeBrain
{	
	VulkanImage::VulkanImage(VulkanRenderContext* context, const ImageConfiguration& configuration)
		: m_Context(context), m_Configuration(configuration)
	{
		NB_PROFILE_FN();

		NB_ASSERT(context, "context null. A valid VulkanRenderContext pointer is required to create VulkanImage.");
		NB_ASSERT(configuration.Width, "Image width 0. Image width must be a non-zero value.");
		NB_ASSERT(configuration.Height, "Image height 0. Image height must be a non-zero value.");
		NB_ASSERT(configuration.Format != ImageFormat::None, "Invalid image format. Image format must be a valid formar.");

		// --- Create Image ---
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = Utils::ImageFormatToVkFormat(m_Configuration.Format);
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
		usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.usage = usage;

		// Allocation
		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		VK_CHECK(vmaCreateImage(m_Context->GetVMAAllocator(), &imageCreateInfo, &allocationCreateInfo, &m_VkImage, &m_VmaAllocation, nullptr));


		// --- Create Image View ---
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = m_VkImage;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
		imageViewCreateInfo.format = Utils::ImageFormatToVkFormat(m_Configuration.Format);

		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(m_Context->GetVkDevice(), &imageViewCreateInfo, nullptr, &m_VkImageView));


		// --- Create sampler ---
		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		samplerCreateInfo.maxAnisotropy = 1;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;

		VK_CHECK(vkCreateSampler(m_Context->GetVkDevice(), &samplerCreateInfo, nullptr, &m_VkSampler));

		m_Context->ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
		{
			Utils::TransitionImage(cmdBuffer, m_VkImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
		});
	}

	VulkanImage::~VulkanImage()
	{
		NB_PROFILE_FN();

		m_Context->WaitForGPU();

		vkDestroySampler(m_Context->GetVkDevice(), m_VkSampler, nullptr);
		m_VkSampler = VK_NULL_HANDLE;

		vkDestroyImageView(m_Context->GetVkDevice(), m_VkImageView, nullptr);
		m_VkImageView = VK_NULL_HANDLE;

		vmaDestroyImage(m_Context->GetVMAAllocator(), m_VkImage, m_VmaAllocation);
		m_VkImage = VK_NULL_HANDLE;
		m_VmaAllocation = VK_NULL_HANDLE;
	}

	uint64_t VulkanImage::GetAddress()
	{
		NB_PROFILE_FN();
		
		if (!m_Address)
			m_Address = (uint64_t)ImGui_ImplVulkan_AddTexture(m_VkSampler, m_VkImageView, VK_IMAGE_LAYOUT_GENERAL);

		return m_Address;
	}
}