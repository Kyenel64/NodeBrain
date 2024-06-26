#include "NBpch.h"
#include "VulkanSwapchain.h"

namespace NodeBrain
{
	static VkSurfaceFormatKHR ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		NB_PROFILE_FN();

		for (const auto& format : availableFormats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				return format;
		}

		NB_ASSERT(false, "No suitable image format for swapchain");
		return { VK_FORMAT_UNDEFINED, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	static VkPresentModeKHR ChooseSwapchainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes)
	{
		NB_PROFILE_FN();

		for (const auto& presentationMode : availablePresentationModes)
		{
			if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return presentationMode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, Window& window)
	{
		NB_PROFILE_FN();

		if (capabilities.currentExtent.width != 0xFFFFFFFF)
		{
			return capabilities.currentExtent;
		}
		else
		{
			glm::vec2 framebufferSize = { window.GetWidth(), window.GetHeight() };
			VkExtent2D actualExtent = { static_cast<uint32_t>(framebufferSize.x), static_cast<uint32_t>(framebufferSize.y) };
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}




	VulkanSwapchain::VulkanSwapchain(Window& window, VkSurfaceKHR surface, VulkanDevice& device, VmaAllocator allocator)
		: m_Window(window), m_VkSurface(surface), m_Device(device), m_ImageIndex(0), m_VmaAllocator(allocator)
	{
		NB_PROFILE_FN();

		VK_CHECK(CreateVkSwapchain());
		VK_CHECK(CreateImageDatas());
		VK_CHECK(CreateFrameDatas());
		VK_CHECK(CreateDrawImage());
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		NB_PROFILE_FN();

		DestroyDrawImage();
		DestroyFrameDatas();
		DestroyImageDatas();
		DestroyVkSwapchain();
	}

	VkResult VulkanSwapchain::CreateVkSwapchain()
	{
		NB_PROFILE_FN();

		// --- Set configurations ---
		SwapchainSupportDetails swapChainSupport = m_Device.GetPhysicalDevice().QuerySwapchainSupport();
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainFormat(swapChainSupport.Formats);
		VkSurfaceCapabilitiesKHR capabilities = swapChainSupport.Capabilities;
		m_VkColorFormat = surfaceFormat.format;
		m_VkColorSpace = surfaceFormat.colorSpace;
		m_VkPresentationMode = ChooseSwapchainPresentationMode(swapChainSupport.PresentationModes);
		m_VkExtent = ChooseSwapExtent(capabilities, m_Window);
		uint32_t minImageCount = (capabilities.maxImageCount > 0 && m_ImageCount > capabilities.maxImageCount) ? capabilities.maxImageCount: capabilities.minImageCount + 1;

		// --- Create swapchain ---
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_VkSurface;
		createInfo.minImageCount = minImageCount;
		createInfo.imageFormat = m_VkColorFormat;
		createInfo.imageColorSpace = m_VkColorSpace;
		createInfo.imageExtent = m_VkExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		QueueFamilyIndices indices = m_Device.GetPhysicalDevice().FindQueueFamilies();
		uint32_t queueFamilyIndices[] = { indices.Graphics.value(), indices.Presentation.value() };

		if (indices.Graphics != indices.Presentation) 
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_VkPresentationMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		return vkCreateSwapchainKHR(m_Device.GetVkDevice(), &createInfo, nullptr, &m_VkSwapchain);
	}

	void VulkanSwapchain::DestroyVkSwapchain()
	{
		NB_PROFILE_FN();

		vkDestroySwapchainKHR(m_Device.GetVkDevice(), m_VkSwapchain, nullptr);
		m_VkSwapchain = VK_NULL_HANDLE;
	}

	VkResult VulkanSwapchain::CreateImageDatas()
	{
		NB_PROFILE_FN();

		// --- Image ---
		// Create temporary contiguous array to get vkImages from swapchain
		vkGetSwapchainImagesKHR(m_Device.GetVkDevice(), m_VkSwapchain, &m_ImageCount, nullptr);
		m_ImageDatas.resize(m_ImageCount);
		std::vector<VkImage> images;
		images.resize(m_ImageCount);
		for (size_t i = 0; i < m_ImageCount; i++)
			images[i] = m_ImageDatas[i].Image;
		vkGetSwapchainImagesKHR(m_Device.GetVkDevice(), m_VkSwapchain, &m_ImageCount, &images[0]);
		for (size_t i = 0; i < m_ImageCount; i++)
			m_ImageDatas[i].Image = images[i];


		// --- Image views ---
		for (size_t i = 0; i < m_ImageCount; i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_ImageDatas[i].Image;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_VkColorFormat;

			// TODO: parameterize
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			VkResult result = vkCreateImageView(m_Device.GetVkDevice(), &imageViewCreateInfo, nullptr, &m_ImageDatas[i].ImageView);
			if (result != VK_SUCCESS)
				return result;
		}

		return VK_SUCCESS;
	}

	void VulkanSwapchain::DestroyImageDatas()
	{
		NB_PROFILE_FN();

		for (size_t i = 0; i < m_ImageCount; i++)
		{
			vkDestroyImageView(m_Device.GetVkDevice(), m_ImageDatas[i].ImageView, nullptr);
			m_ImageDatas[i].ImageView = VK_NULL_HANDLE;
		}
	}

	VkResult VulkanSwapchain::CreateFrameDatas()
	{
		NB_PROFILE_FN();

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			// CommandPool
			QueueFamilyIndices queueFamilyIndices = m_Device.GetPhysicalDevice().FindQueueFamilies();
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.Graphics.value();
			VkResult result = vkCreateCommandPool(m_Device.GetVkDevice(), &commandPoolCreateInfo, nullptr, &m_FrameDatas[i].CommandPool);
			if (result != VK_SUCCESS)
				return result;

			// CommandBuffer
			VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.commandPool = m_FrameDatas[i].CommandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
			result = vkAllocateCommandBuffers(m_Device.GetVkDevice(), &commandBufferAllocateInfo, &m_FrameDatas[i].CommandBuffer);
			if (result != VK_SUCCESS)
				return result;

			// Semaphores
			VkSemaphoreCreateInfo semaphoreCreateInfo = {};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			result = vkCreateSemaphore(m_Device.GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_FrameDatas[i].ImageAvailableSemaphore);
			if (result != VK_SUCCESS)
				return result;
			result = vkCreateSemaphore(m_Device.GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_FrameDatas[i].RenderFinishedSemaphore);
			if (result != VK_SUCCESS)
				return result;

			// Fence
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			result = vkCreateFence(m_Device.GetVkDevice(), &fenceCreateInfo, nullptr, &m_FrameDatas[i].InFlightFence);
			if (result != VK_SUCCESS)
				return result;
		}
	
		return VK_SUCCESS;
	}

	void VulkanSwapchain::DestroyFrameDatas()
	{
		NB_PROFILE_FN();

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyFence(m_Device.GetVkDevice(), m_FrameDatas[i].InFlightFence, nullptr);
			m_FrameDatas[i].InFlightFence = VK_NULL_HANDLE;
			vkDestroySemaphore(m_Device.GetVkDevice(), m_FrameDatas[i].RenderFinishedSemaphore, nullptr);
			m_FrameDatas[i].RenderFinishedSemaphore = VK_NULL_HANDLE;
			vkDestroySemaphore(m_Device.GetVkDevice(), m_FrameDatas[i].ImageAvailableSemaphore, nullptr);
			m_FrameDatas[i].ImageAvailableSemaphore = VK_NULL_HANDLE;
			vkDestroyCommandPool(m_Device.GetVkDevice(), m_FrameDatas[i].CommandPool, nullptr);
			m_FrameDatas[i].CommandPool = VK_NULL_HANDLE;
		}
	}

	VkResult VulkanSwapchain::CreateDrawImage()
	{
		NB_PROFILE_FN();

		VkResult result;

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			// --- Create Draw Image ---
			VkImageCreateInfo drawImageCreateInfo = {};
			drawImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			drawImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			drawImageCreateInfo.format = Utils::ImageFormatToVkFormat(ImageFormat::RGBA16);
			drawImageCreateInfo.extent = { m_VkExtent.width, m_VkExtent.height, 1 };
			drawImageCreateInfo.mipLevels = 1;
			drawImageCreateInfo.arrayLayers = 1;
			drawImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			drawImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

			// Usage flags
			VkImageUsageFlags usage = {};
			usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			usage |= VK_IMAGE_USAGE_STORAGE_BIT;
			usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
			drawImageCreateInfo.usage = usage;

			// Allocation
			VmaAllocationCreateInfo allocationCreateInfo = {};
			allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
			allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			result = vmaCreateImage(m_VmaAllocator, &drawImageCreateInfo, &allocationCreateInfo, &m_DrawImageDatas[i].Image, &m_DrawImageDatas[i].Allocation, nullptr);
			if (result != VK_SUCCESS)
				return result;


			// --- Create Draw ImageView ---
			VkImageViewCreateInfo drawImageViewCreateInfo = {};
			drawImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			drawImageViewCreateInfo.image = m_DrawImageDatas[i].Image;
			drawImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: parameterize
			drawImageViewCreateInfo.format = Utils::ImageFormatToVkFormat(ImageFormat::RGBA16);

			drawImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			drawImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			drawImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			drawImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			drawImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			drawImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			drawImageViewCreateInfo.subresourceRange.levelCount = 1;
			drawImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			drawImageViewCreateInfo.subresourceRange.layerCount = 1;

			result = vkCreateImageView(m_Device.GetVkDevice(), &drawImageViewCreateInfo, nullptr, &m_DrawImageDatas[i].ImageView);
			if (result != VK_SUCCESS)
				return result;
		}

		return VK_SUCCESS;
	}

	void VulkanSwapchain::DestroyDrawImage()
	{
		NB_PROFILE_FN();

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyImageView(m_Device.GetVkDevice(), m_DrawImageDatas[i].ImageView, nullptr);
			m_DrawImageDatas[i].ImageView = VK_NULL_HANDLE;

			vmaDestroyImage(m_VmaAllocator, m_DrawImageDatas[i].Image, m_DrawImageDatas[i].Allocation);
			m_DrawImageDatas[i].Image = VK_NULL_HANDLE;
			m_DrawImageDatas[i].Allocation = VK_NULL_HANDLE;
		}
	}

	uint32_t VulkanSwapchain::AcquireNextImage()
	{
		NB_PROFILE_FN();

		vkWaitForFences(m_Device.GetVkDevice(), 1, &m_FrameDatas[m_FrameIndex].InFlightFence, VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(m_Device.GetVkDevice(), m_VkSwapchain, UINT64_MAX, m_FrameDatas[m_FrameIndex].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
			RecreateSwapchain();
			
		vkResetFences(m_Device.GetVkDevice(), 1, &m_FrameDatas[m_FrameIndex].InFlightFence);
		return m_ImageIndex;
	}

	void VulkanSwapchain::PresentImage()
	{
		NB_PROFILE_FN();

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_FrameDatas[m_FrameIndex].RenderFinishedSemaphore;

		VkSwapchainKHR swapChains[] = { m_VkSwapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_ImageIndex;
		presentInfo.pResults = nullptr; // Optional

		VkResult result = vkQueuePresentKHR(m_Device.GetPresentationQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			RecreateSwapchain();

		m_FrameIndex = (m_FrameIndex + 1) % FRAMES_IN_FLIGHT;
	}

	void VulkanSwapchain::RecreateSwapchain()
	{
		NB_PROFILE_FN();

		vkDeviceWaitIdle(m_Device.GetVkDevice());

		// --- Destroy ---
		DestroyDrawImage();
		DestroyImageDatas();
		DestroyVkSwapchain();


		// --- Recreate ---
		VK_CHECK(CreateVkSwapchain());
		VK_CHECK(CreateImageDatas());
		VK_CHECK(CreateDrawImage());

		NB_INFO("Recreated swapchain to size: {0}, {1}", m_VkExtent.width, m_VkExtent.height);
	}
}