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

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentationModes;
	};

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(VkInstance instance, uint32_t deviceIndex, VkSurfaceKHR surface = VK_NULL_HANDLE);
		~VulkanPhysicalDevice() = default;

		bool IsSuitable();

		VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
		const std::vector<const char*>& GetDeviceExtensions() const { return m_DeviceExtensions; }
		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		const SwapChainSupportDetails& GetSwapChainSupportDetails() const { return m_SwapChainSupportDetails; }

	private:
		void Init();

		QueueFamilyIndices FindQueueFamilies();
		bool CheckDeviceExtensionSupport();
		SwapChainSupportDetails QuerySwapChainSupport();

	private:
		VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurfaceKHR = VK_NULL_HANDLE;

		QueueFamilyIndices m_QueueFamilyIndices;
		SwapChainSupportDetails m_SwapChainSupportDetails;

		std::vector<const char*> m_DeviceExtensions;
		uint32_t m_DeviceIndex;
	};
}