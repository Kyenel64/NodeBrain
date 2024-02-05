#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanPhysicalDevice.h"

namespace NodeBrain
{
	class VulkanDevice
	{
	public:
		VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physicalDevice);
		~VulkanDevice();

		VkDevice GetVkDevice() const { return m_Device; }
		std::shared_ptr<VulkanPhysicalDevice> GetPhysicalDevice() const { return m_PhysicalDevice; }

	private:
		void Init();

	private:
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentationQueue = VK_NULL_HANDLE;	
		std::vector<const char*> m_ValidationLayers;
	};
}