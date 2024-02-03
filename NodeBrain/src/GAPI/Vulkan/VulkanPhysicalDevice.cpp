#include "NBpch.h"
#include "VulkanPhysicalDevice.h"

namespace NodeBrain
{
    VulkanPhysicalDevice::VulkanPhysicalDevice(VkInstance instance, uint32_t deviceHandle)
        : m_VkInstance(instance)
    {
        PickPhysicalDevice(deviceHandle);
    }

    void VulkanPhysicalDevice::PickPhysicalDevice(uint32_t deviceNumber)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);
        NB_ASSERT(deviceCount, "Could not find any GPUs with Vulkan support");
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, &devices[0]);

        for (const auto& device : devices)
        {
            // Use first suitable device for now. 
            if (IsDeviceSuitable(device))
            {
                m_Device = device;
                break;
            }
        }
        NB_ASSERT(m_Device != VK_NULL_HANDLE, "Could not find suitable GPU");

    }

    bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device)
    {
        if (device == VK_NULL_HANDLE)
            return false;
        QueueFamilyIndices indices = FindQueueFamilies(device);
        return indices.IsComplete();
    }

    QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, &queueFamilies[0]);

        for (int i = 0; i < queueFamilyCount; i++)
        {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.Graphics = i;
        }

        return indices;
    }
}