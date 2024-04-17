#include "NBpch.h"
#include "VulkanSwapchain.h"

#include "Core/App.h"

namespace NodeBrain
{
	namespace Utils
	{
		static VkSurfaceFormatKHR ChooseSwapchainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			for (const auto& format : availableFormats)
			{
				if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
					return format;
			}

			return availableFormats[0];
		}

		static VkPresentModeKHR ChooseSwapchainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes)
		{
			for (const auto& presentationMode : availablePresentationModes)
			{
				if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return presentationMode;
			}
			return VK_PRESENT_MODE_FIFO_KHR;
		}

		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}
			else
			{
				glm::vec2 framebufferSize = App::Get()->GetWindow().GetFramebufferSize();
				VkExtent2D actualExtent = { static_cast<uint32_t>(framebufferSize.x), static_cast<uint32_t>(framebufferSize.y) };
				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}
	}




	VulkanSwapchain::VulkanSwapchain(VkSurfaceKHR surface, std::shared_ptr<VulkanDevice> device)
		: m_VkSurface(surface), m_Device(device), m_ImageIndex(0)
	{
		NB_PROFILE_FN();

		VK_CHECK(CreateVkSwapchain());
		VK_CHECK(CreateVkRenderPass());
		VK_CHECK(CreateImageDatas());
		VK_CHECK(CreateFrameDatas());

		ImageConfiguration config = {};
		config.Width = m_VkExtent.width;
		config.Height = m_VkExtent.height;
		config.Format = ImageFormat::RGBA16;
		m_DrawImage = std::make_shared<VulkanImage>(config);
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		NB_PROFILE_FN();

		vkDeviceWaitIdle(m_Device->GetVkDevice());

		m_DrawImage.reset();
		DestroyFrameDatas();
		DestroyImageDatas();
		DestroyVkRenderPass();
		DestroyVkSwapchain();
	}

	VkResult VulkanSwapchain::CreateVkSwapchain()
	{
		// --- Set configurations ---
		SwapchainSupportDetails swapChainSupport = m_Device->GetPhysicalDevice()->QuerySwapchainSupport();
		VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapchainFormat(swapChainSupport.Formats);
		VkSurfaceCapabilitiesKHR capabilities = swapChainSupport.Capabilities;
		m_VkColorFormat = surfaceFormat.format;
		m_VkColorSpace = surfaceFormat.colorSpace;
		m_VkPresentationMode = Utils::ChooseSwapchainPresentationMode(swapChainSupport.PresentationModes);
		m_VkExtent = Utils::ChooseSwapExtent(capabilities);
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

		QueueFamilyIndices indices = m_Device->GetPhysicalDevice()->FindQueueFamilies();
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

		return vkCreateSwapchainKHR(m_Device->GetVkDevice(), &createInfo, nullptr, &m_VkSwapchain);
	}

	void VulkanSwapchain::DestroyVkSwapchain()
	{
		if (m_VkSwapchain)
		{
			vkDestroySwapchainKHR(m_Device->GetVkDevice(), m_VkSwapchain, nullptr);
			m_VkSwapchain = VK_NULL_HANDLE;
		}
	}

	VkResult VulkanSwapchain::CreateImageDatas()
	{
		// --- Image ---
		// Create temporary contiguous array to get vkImages from swapchain
		vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_VkSwapchain, &m_ImageCount, nullptr);
		m_ImageDatas.resize(m_ImageCount);
		std::vector<VkImage> images;
		images.resize(m_ImageCount);
		for (size_t i = 0; i < m_ImageCount; i++)
			images[i] = m_ImageDatas[i].Image;
		vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_VkSwapchain, &m_ImageCount, &images[0]);
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

			VkResult result = vkCreateImageView(m_Device->GetVkDevice(), &imageViewCreateInfo, nullptr, &m_ImageDatas[i].ImageView);
			if (result != VK_SUCCESS)
				return result;
		}

		// --- Create framebuffers ---
		for (int i = 0; i < m_ImageCount; i++)
		{
			VkImageView attachments[] = { m_ImageDatas[i].ImageView };

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = m_VkRenderPass;
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = attachments;
			framebufferCreateInfo.width = m_VkExtent.width;
			framebufferCreateInfo.height = m_VkExtent.height;
			framebufferCreateInfo.layers = 1;

			VkResult result = vkCreateFramebuffer(m_Device->GetVkDevice(), &framebufferCreateInfo, nullptr, &m_ImageDatas[i].Framebuffer);
			if (result != VK_SUCCESS)
				return result;
		}

		return VK_SUCCESS;
	}

	void VulkanSwapchain::DestroyImageDatas()
	{
		for (size_t i = 0; i < m_ImageCount; i++)
		{
			vkDestroyImageView(m_Device->GetVkDevice(), m_ImageDatas[i].ImageView, nullptr);
			m_ImageDatas[i].ImageView = VK_NULL_HANDLE;
			vkDestroyFramebuffer(m_Device->GetVkDevice(), m_ImageDatas[i].Framebuffer, nullptr);
			m_ImageDatas[i].Framebuffer = VK_NULL_HANDLE;
		}
	}

	VkResult VulkanSwapchain::CreateFrameDatas()
	{
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			// CommandPool
			QueueFamilyIndices queueFamilyIndices = m_Device->GetPhysicalDevice()->FindQueueFamilies();
			VkCommandPoolCreateInfo commandPoolCreateInfo = {};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.Graphics.value();
			VkResult result = vkCreateCommandPool(m_Device->GetVkDevice(), &commandPoolCreateInfo, nullptr, &m_FrameDatas[i].CommandPool);
			if (result != VK_SUCCESS)
				return result;

			// CommandBuffer
			VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
			commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfo.commandPool = m_FrameDatas[i].CommandPool;
			commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfo.commandBufferCount = 1;
			result = vkAllocateCommandBuffers(m_Device->GetVkDevice(), &commandBufferAllocateInfo, &m_FrameDatas[i].CommandBuffer);
			if (result != VK_SUCCESS)
				return result;

			// Semaphores
			VkSemaphoreCreateInfo semaphoreCreateInfo = {};
			semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			result = vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_FrameDatas[i].ImageAvailableSemaphore);
			if (result != VK_SUCCESS)
				return result;
			result = vkCreateSemaphore(m_Device->GetVkDevice(), &semaphoreCreateInfo, nullptr, &m_FrameDatas[i].RenderFinishedSemaphore);
			if (result != VK_SUCCESS)
				return result;

			// Fence
			VkFenceCreateInfo fenceCreateInfo = {};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			result = vkCreateFence(m_Device->GetVkDevice(), &fenceCreateInfo, nullptr, &m_FrameDatas[i].InFlightFence);
			if (result != VK_SUCCESS)
				return result;
		}
	
		return VK_SUCCESS;
	}

	void VulkanSwapchain::DestroyFrameDatas()
	{
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyFence(m_Device->GetVkDevice(), m_FrameDatas[i].InFlightFence, nullptr);
			m_FrameDatas[i].InFlightFence = VK_NULL_HANDLE;
			vkDestroySemaphore(m_Device->GetVkDevice(), m_FrameDatas[i].RenderFinishedSemaphore, nullptr);
			m_FrameDatas[i].RenderFinishedSemaphore = VK_NULL_HANDLE;
			vkDestroySemaphore(m_Device->GetVkDevice(), m_FrameDatas[i].ImageAvailableSemaphore, nullptr);
			m_FrameDatas[i].ImageAvailableSemaphore = VK_NULL_HANDLE;
			vkDestroyCommandPool(m_Device->GetVkDevice(), m_FrameDatas[i].CommandPool, nullptr);
			m_FrameDatas[i].CommandPool = VK_NULL_HANDLE;
		}
	}

	VkResult VulkanSwapchain::CreateVkRenderPass()
	{
		// Color attachment
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB; // temp. parametrize
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Color attachment subpass
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		// Render pass
		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachment;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpass;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &dependency;

		return vkCreateRenderPass(m_Device->GetVkDevice(), &renderPassCreateInfo, nullptr, &m_VkRenderPass);
	}

	void VulkanSwapchain::DestroyVkRenderPass()
	{
		if (m_VkRenderPass)
		{
			vkDestroyRenderPass(m_Device->GetVkDevice(), m_VkRenderPass, nullptr);
			m_VkRenderPass = VK_NULL_HANDLE;
		}
	}

	uint32_t VulkanSwapchain::AcquireNextImage()
	{
		vkWaitForFences(m_Device->GetVkDevice(), 1, &m_FrameDatas[m_CurrentFrame].InFlightFence, VK_TRUE, UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(m_Device->GetVkDevice(), m_VkSwapchain, UINT64_MAX, m_FrameDatas[m_CurrentFrame].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_ImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapchain();
			NB_INFO("Swapchain recreated. New extent: {0}, {1}", m_VkExtent.width, m_VkExtent.height); 
		}
			
		vkResetFences(m_Device->GetVkDevice(), 1, &m_FrameDatas[m_CurrentFrame].InFlightFence);
		return m_ImageIndex;
	}

	void VulkanSwapchain::PresentImage()
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_FrameDatas[m_CurrentFrame].RenderFinishedSemaphore;

		VkSwapchainKHR swapChains[] = { m_VkSwapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_ImageIndex;
		presentInfo.pResults = nullptr; // Optional

		VkResult result = vkQueuePresentKHR(m_Device->GetPresentationQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapchain();
			NB_INFO("Swapchain recreated. New extent: {0}, {1}", m_VkExtent.width, m_VkExtent.height); 
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_IN_FLIGHT;
	}

	void VulkanSwapchain::CopyImage(std::shared_ptr<VulkanImage> image)
	{

	}

	void VulkanSwapchain::RecreateSwapchain()
	{
		vkDeviceWaitIdle(m_Device->GetVkDevice());

		// --- Destroy ---
		DestroyImageDatas();
		DestroyVkSwapchain();


		// --- Recreate ---
		VK_CHECK(CreateVkSwapchain());
		VK_CHECK(CreateImageDatas());

		NB_TRACE("Recreated swapchain to size: {0}, {1}", m_VkExtent.width, m_VkExtent.height);
	}
}