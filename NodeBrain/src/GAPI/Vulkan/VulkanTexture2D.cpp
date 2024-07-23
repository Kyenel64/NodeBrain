#include "NBpch.h"
#include "VulkanTexture2D.h"

#include <ImGui/backends/imgui_impl_vulkan.h>
#include <stb/stb_image.h>

namespace NodeBrain
{
	VulkanTexture2D::VulkanTexture2D(VulkanRenderContext& context, const Texture2DConfiguration& configuration)
		: m_Context(context), m_Configuration(configuration)
	{
		NB_PROFILE_FN();

		NB_ASSERT(m_Configuration.Width, "Image width 0. Image width must be a non-zero value.");
		NB_ASSERT(m_Configuration.Height, "Image height 0. Image height must be a non-zero value.");
		NB_ASSERT(m_Configuration.Format != ImageFormat::None, "Invalid image format. Image format must be a valid formar.");

		VK_CHECK(Create(m_Configuration.Width, m_Configuration.Height, m_Configuration.Format));
	}

	VulkanTexture2D::VulkanTexture2D(VulkanRenderContext& context, std::filesystem::path path)
		: m_Context(context), m_Path(std::move(path))
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		const unsigned char* data = stbi_load(m_Path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		NB_ASSERT(data, "Failed to load image");

		m_Configuration.Format = ImageFormat::RGBA8;
		m_Configuration.Width = width;
		m_Configuration.Height = height;

		VK_CHECK(Create(m_Configuration.Width, m_Configuration.Height, ImageFormat::RGBA8));

		m_Context.ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
		{
			for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				memcpy(m_StagingMappedData[i], data, width * height * 4);
				Utils::TransitionImage(cmdBuffer, m_VkImage[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset = 0;
				copyRegion.bufferRowLength = 0;
				copyRegion.bufferImageHeight = 0;

				copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel = 0;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount = 1;
				copyRegion.imageExtent = { m_Configuration.Width, m_Configuration.Height, 1 };

				// copy the buffer into the image
				vkCmdCopyBufferToImage(cmdBuffer, m_VkStagingBuffer[i], m_VkImage[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

				Utils::TransitionImage(cmdBuffer, m_VkImage[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}
		});
	}

	VkResult VulkanTexture2D::Create(uint32_t width, uint32_t height, ImageFormat format)
	{
		// --- Create Image ---
		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB; // TODO: Figure out proper color format
		imageCreateInfo.extent = { width, height, 1 };
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

		// Usage flags
		VkImageUsageFlags usage = {};
		//usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		usage |= VK_IMAGE_USAGE_STORAGE_BIT;
		//usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.usage = usage;

		// Allocation
		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		// --- Create Staging Buffer ---
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = width * height * 4; // TODO: 4 for RGBA8 8 for RGBA16?
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo stagingAllocInfo = {};
		stagingAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		stagingAllocInfo.flags = 0;


		// --- Create Image View ---
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
		imageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB; // TODO: Figure out proper color format

		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

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


		VkResult result;
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			result = vmaCreateImage(m_Context.GetVMAAllocator(), &imageCreateInfo, &allocationCreateInfo, &m_VkImage[i], &m_VmaAllocation[i], nullptr);
			if (result != VK_SUCCESS)
				return result;
			imageViewCreateInfo.image = m_VkImage[i];
			result = vkCreateImageView(m_Context.GetVkDevice(), &imageViewCreateInfo, nullptr, &m_VkImageView[i]);
			if (result != VK_SUCCESS)
				return result;

			result = vmaCreateBuffer(m_Context.GetVMAAllocator(), &bufferCreateInfo, &stagingAllocInfo, &m_VkStagingBuffer[i], &m_VmaStagingAllocation[i], nullptr);
			if (result != VK_SUCCESS)
				return result;
			result = vmaMapMemory(m_Context.GetVMAAllocator(), m_VmaStagingAllocation[i], &m_StagingMappedData[i]);
			if (result != VK_SUCCESS)
				return result;
			result = vkCreateSampler(m_Context.GetVkDevice(), &samplerCreateInfo, nullptr, &m_VkSampler[i]);
			if (result != VK_SUCCESS)
				return result;

			m_Context.ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
			{
				Utils::TransitionImage(cmdBuffer, m_VkImage[i], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			});
		}

		return VK_SUCCESS;
	}

	void VulkanTexture2D::Destroy()
	{
		m_Context.WaitForGPU();

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vmaUnmapMemory(m_Context.GetVMAAllocator(), m_VmaStagingAllocation[i]);
			m_StagingMappedData[i] = nullptr;
			vmaDestroyBuffer(m_Context.GetVMAAllocator(), m_VkStagingBuffer[i], m_VmaStagingAllocation[i]);
			m_VkStagingBuffer[i] = VK_NULL_HANDLE;
			m_VmaStagingAllocation[i] = VK_NULL_HANDLE;

			vkDestroySampler(m_Context.GetVkDevice(), m_VkSampler[i], nullptr);
			m_VkSampler[i] = VK_NULL_HANDLE;

			vkDestroyImageView(m_Context.GetVkDevice(), m_VkImageView[i], nullptr);
			m_VkImageView[i] = VK_NULL_HANDLE;

			vmaDestroyImage(m_Context.GetVMAAllocator(), m_VkImage[i], m_VmaAllocation[i]);
			m_VkImage[i] = VK_NULL_HANDLE;
			m_VmaAllocation[i] = VK_NULL_HANDLE;
		}
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		NB_PROFILE_FN();

		Destroy();
	}

	void VulkanTexture2D::SetData(const void* data, uint32_t size)
	{
		m_Context.ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
		{
			for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
			{
				memcpy(m_StagingMappedData[i], data, size);
				Utils::TransitionImage(cmdBuffer, m_VkImage[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

				VkBufferImageCopy copyRegion = {};
				copyRegion.bufferOffset = 0;
				copyRegion.bufferRowLength = 0;
				copyRegion.bufferImageHeight = 0;

				copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				copyRegion.imageSubresource.mipLevel = 0;
				copyRegion.imageSubresource.baseArrayLayer = 0;
				copyRegion.imageSubresource.layerCount = 1;
				copyRegion.imageExtent = { m_Configuration.Width, m_Configuration.Height, 1 };

				// copy the buffer into the image
				vkCmdCopyBufferToImage(cmdBuffer, m_VkStagingBuffer[i], m_VkImage[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

				Utils::TransitionImage(cmdBuffer, m_VkImage[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			}
		});
	}

	uint64_t VulkanTexture2D::GetAddress()
	{
		NB_PROFILE_FN();

		if (!m_Address)
			m_Address = (uint64_t)ImGui_ImplVulkan_AddTexture(m_VkSampler[m_Context.GetSwapchain().GetFrameIndex()], m_VkImageView[m_Context.GetSwapchain().GetFrameIndex()], VK_IMAGE_LAYOUT_GENERAL);

		return m_Address;
	}
}
