#pragma once

#include <vulkan/vulkan.h>

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

	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentationModes;
	};

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, uint32_t deviceIndex);
		~VulkanPhysicalDevice() = default;

		bool IsSuitable();

		VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
		const std::vector<const char*>& GetDeviceExtensions() const { return m_DeviceExtensions; }
		QueueFamilyIndices FindQueueFamilies() const;
		SwapchainSupportDetails QuerySwapchainSupport() const;
		bool CheckDeviceExtensionSupport() const;

	private:
		void Init();

	private:
		VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurfaceKHR = VK_NULL_HANDLE;

		uint32_t m_DeviceIndex;
		std::vector<const char*> m_DeviceExtensions;
	};
}