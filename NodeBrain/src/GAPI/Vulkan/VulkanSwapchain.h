#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanImage.h"

namespace NodeBrain
{
	#define FRAMES_IN_FLIGHT 2

	// Contains data per swapchain image
	struct ImageData
	{
		VkImage Image = VK_NULL_HANDLE;
		VkImageView ImageView = VK_NULL_HANDLE;
	};

	// Contains data per frame in flight
	struct FrameData
	{
		VkCommandPool CommandPool = VK_NULL_HANDLE;
		VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
		VkSemaphore ImageAvailableSemaphore = VK_NULL_HANDLE;
		VkSemaphore RenderFinishedSemaphore = VK_NULL_HANDLE;
		VkFence InFlightFence = VK_NULL_HANDLE;
	};



	class VulkanSwapchain
	{
	public:
		VulkanSwapchain(VkSurfaceKHR surface, VulkanDevice& device);
		~VulkanSwapchain();

		uint32_t AcquireNextImage();
		void PresentImage();

		VkSwapchainKHR GetVkSwapchain() const { return m_VkSwapchain; }
		std::shared_ptr<VulkanImage> GetDrawImage() const { return m_DrawImage; }	
		const FrameData& GetCurrentFrameData() const { return m_FrameDatas[m_FrameIndex]; }
		const ImageData& GetCurrentImageData() const { return m_ImageDatas[m_ImageIndex]; }	
		uint32_t GetImageIndex() const { return m_ImageIndex; }
		uint32_t GetFrameIndex() const { return m_FrameIndex; }
		VkExtent2D GetVkExtent() const { return m_VkExtent; }
		VkFormat GetVkFormat() const { return m_VkColorFormat; }
		uint32_t GetImageCount() const { return m_ImageCount; }

	private:
		void RecreateSwapchain();

		VkResult CreateVkSwapchain();
		VkResult CreateImageDatas();
		VkResult CreateFrameDatas();

		void DestroyVkSwapchain();
		void DestroyImageDatas();
		void DestroyFrameDatas();

	private:
		VkSwapchainKHR m_VkSwapchain = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;
		VulkanDevice& m_Device;

		// Draw Image
		std::shared_ptr<VulkanImage> m_DrawImage;

		// Configuration
		VkExtent2D m_VkExtent;
		VkFormat m_VkColorFormat;
		VkColorSpaceKHR m_VkColorSpace;
		VkPresentModeKHR m_VkPresentationMode;
		uint32_t m_ImageCount = 0;

		std::vector<ImageData> m_ImageDatas;
		FrameData m_FrameDatas[FRAMES_IN_FLIGHT];

		// Used for ImageData indexing. Index of current swapchain image retrieved by the GPU. 
		// This can be any index from the swapchain that is ready to use.
		uint32_t m_ImageIndex = 0;
		// Used for FrameData indexing. Index of current processing frame.
		// This loops through the number of frames in flight in ascending order.
		uint32_t m_FrameIndex = 0;
	};
}