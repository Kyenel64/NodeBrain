#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/RendererAPI.h"
#include "GAPI/Vulkan/ValidationLayer.h"

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
		VkInstance m_VkInstance = nullptr;
		bool m_EnableValidationLayers = false;
		std::unique_ptr<ValidationLayer> m_ValidationLayer;
	};
}