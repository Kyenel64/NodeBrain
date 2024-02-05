#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanSurface.h"

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

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(uint32_t deviceNumber);
		~VulkanPhysicalDevice() = default;

		QueueFamilyIndices GetQueueFamilyIndices() { return FindQueueFamilies(m_PhysicalDevice); }
		SwapChainSupportDetails GetSwapChainSupportDetails() { return QuerySwapChainSupport(m_PhysicalDevice); }
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
		const std::vector<const char*>& GetDeviceExtensions() const { return m_DeviceExtensions; }

		static VkSurfaceFormatKHR ChooseSwapChainFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR ChooseSwapChainPresentationMode(const std::vector<VkPresentModeKHR>& availablePresentationModes);
		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:
		void PickPhysicalDevice(uint32_t deviceHandle);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		
	private:
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		std::shared_ptr<VulkanSurface> m_Surface;
		std::vector<const char*> m_DeviceExtensions;
	};
}