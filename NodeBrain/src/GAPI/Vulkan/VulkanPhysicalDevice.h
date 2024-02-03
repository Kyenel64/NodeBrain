#pragma once

#include <vulkan/vulkan.h>

namespace NodeBrain
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> Graphics;

		bool IsComplete()
		{
			return Graphics.has_value();
		}
	};

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(VkInstance instance, uint32_t deviceNumber = 0);
		~VulkanPhysicalDevice() = default;

		QueueFamilyIndices GetQueueFamilyIndices() { return FindQueueFamilies(m_PhysicalDevice); }
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }

	private:
		void PickPhysicalDevice(uint32_t deviceHandle);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		
	private:
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	};
}