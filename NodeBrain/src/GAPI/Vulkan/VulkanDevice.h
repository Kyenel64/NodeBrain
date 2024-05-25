#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanPhysicalDevice.h"

namespace NodeBrain
{
	class VulkanDevice
	{
	public:
		VulkanDevice(VulkanPhysicalDevice& physicalDevice, const std::vector<const char*>& enabledLayers);
		~VulkanDevice();

		VkDevice GetVkDevice() const { return m_VkDevice; }
		VulkanPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
		VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		VkQueue GetPresentationQueue() const { return m_PresentationQueue; }

	private:
		VulkanPhysicalDevice& m_PhysicalDevice;
		VkDevice m_VkDevice = VK_NULL_HANDLE;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentationQueue = VK_NULL_HANDLE;	
	};
}