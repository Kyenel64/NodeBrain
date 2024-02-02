#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/RendererAPI.h"

namespace NodeBrain
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;

		virtual void Shutdown() override;

	private:
		VkResult CreateInstance();
		void SetupDebugger();
		bool CheckExtensionSupport(std::vector<const char*> extensions);
		bool CheckValidationLayerSupport();

	private:
		VkInstance m_VkInstance = nullptr;
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
		bool m_EnableValidationLayers = false;
		std::vector<const char*> m_ValidationLayers;
	};
}