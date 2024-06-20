#pragma once

#include <vulkan/vulkan.h>

namespace NodeBrain
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> Graphics;
		std::optional<uint32_t> Presentation;

		[[nodiscard]] bool IsComplete() const
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

		[[nodiscard]] bool IsSuitable() const;

		[[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
		[[nodiscard]] const std::vector<const char*>& GetEnabledDeviceExtensions() const { return m_EnabledDeviceExtensions; }
		
		[[nodiscard]] QueueFamilyIndices FindQueueFamilies() const;
		[[nodiscard]] SwapchainSupportDetails QuerySwapchainSupport() const;
		[[nodiscard]] bool CheckDeviceExtensionSupport() const;

	private:
		VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkSurfaceKHR m_VkSurfaceKHR = VK_NULL_HANDLE;

		uint32_t m_PhysicalDeviceIndex;
		std::vector<const char*> m_EnabledDeviceExtensions;
	};
}