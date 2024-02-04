#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/RendererAPI.h"
#include "GAPI/Vulkan/VulkanValidationLayer.h"
#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanSurface.h"

namespace NodeBrain
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI();
		virtual ~VulkanRendererAPI();

	private:
		void Init();
		VkResult CreateInstance();
		bool CheckExtensionSupport(std::vector<const char*> extensions);

	private:
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		bool m_EnableValidationLayers = false;
		std::shared_ptr<VulkanValidationLayer> m_ValidationLayer;
		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::shared_ptr<VulkanDevice> m_Device;
		std::shared_ptr<VulkanSurface> m_Surface;
	};
}