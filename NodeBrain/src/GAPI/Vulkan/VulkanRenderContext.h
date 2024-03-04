#pragma once

#include <vulkan/vulkan.h>

#include "Core/Window.h"
#include "Renderer/RenderContext.h"

namespace NodeBrain
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> Graphics;
		std::optional<uint32_t> Presentation;

		bool IsComplete() const
		{
			return Graphics.has_value() && Presentation.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentationModes;
	};



	// --- VulkanRenderContext ------------------------------------------------
	class VulkanRenderContext : public RenderContext
	{
	public:
		VulkanRenderContext(Window* window);
		~VulkanRenderContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		// Getters
		static VulkanRenderContext* GetInstance();
		VkInstance GetVkInstance() const { return m_VkInstance; }
		VkDevice GetVkDevice() const { return m_VkDevice; }

	private:
		// Create Vulkan objects
		void CreateInstance();
		void CreateDebugUtilsMessenger();
		void CreatePhysicalDevice();
		void CreateDevice();
		void CreateSwapchain();
		void CreateImageViews();

	private:
		Window* m_Window = nullptr;
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurface = VK_NULL_HANDLE;

		// Physical Device
		VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
		QueueFamilyIndices m_QueueFamilyIndices;
		SwapChainSupportDetails m_SwapChainSupportDetails;
		std::vector<const char*> m_PhysicalDeviceExtensions;

		// Device
		VkDevice m_VkDevice = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentationQueue = VK_NULL_HANDLE;	

		// Swapchain
		VkSwapchainKHR m_VkSwapchain;
		std::vector<VkImage> m_SwapchainImages;
		std::vector<VkImageView> m_SwapchainImageViews;
		// Swapchain Configuration
		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;
		VkExtent2D m_Extent;
		VkPresentModeKHR m_PresentationMode;

		// Debug
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
		std::vector<const char*> m_ValidationLayers;
	};
}