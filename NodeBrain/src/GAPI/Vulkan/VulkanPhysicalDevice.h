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

    private:
        void PickPhysicalDevice(uint32_t deviceHandle);
        bool IsDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		
    private:
        VkInstance m_VkInstance = VK_NULL_HANDLE;
        VkPhysicalDevice m_Device = VK_NULL_HANDLE;
    };
}