#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanValidationLayer.h"
#include "GAPI/Vulkan/VulkanPhysicalDevice.h"

namespace NodeBrain
{
	class VulkanDevice
	{
	public:
		VulkanDevice(std::shared_ptr<VulkanPhysicalDevice> physicalDevice, std::shared_ptr<VulkanValidationLayer> validationLayer);
		~VulkanDevice();

	private:
		void Init();

	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::shared_ptr<VulkanValidationLayer> m_ValidationLayer;
		bool m_EnableValidationLayers = false;
	};
}