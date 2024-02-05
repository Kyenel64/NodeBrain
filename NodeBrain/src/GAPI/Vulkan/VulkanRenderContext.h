#pragma once

#include <vulkan/vulkan.h>

#include "Core/Window.h"
#include "Renderer/RenderContext.h"
#include "GAPI/Vulkan/VulkanPhysicalDevice.h"
#include "GAPI/Vulkan/VulkanDevice.h"
#include "GAPI/Vulkan/VulkanSurface.h"
#include "GAPI/Vulkan/VulkanSwapChain.h"

namespace NodeBrain
{
    class VulkanRenderContext : public RenderContext
    {
    public:
		VulkanRenderContext(Window* window);
		~VulkanRenderContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;

		static VulkanRenderContext* GetInstance();
		VkInstance GetVkInstance() const { return m_VkInstance; }
		const std::vector<const char*>& GetValidationLayers() const { return m_ValidationLayers; }
		std::shared_ptr<VulkanSurface> GetSurface() const { return m_Surface; }
		bool IsValidationLayersEnabled() const { return m_EnableValidationLayers; }

	private:
		VkResult CreateInstance();
		VkResult CreateDebugUtilsMessenger();

		bool CheckExtensionSupport(std::vector<const char*> extensions);
		bool CheckValidationLayerSupport();

    private:
		Window* m_Window = nullptr;
		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
		bool m_EnableValidationLayers = false;
		std::vector<const char*> m_ValidationLayers;

		std::shared_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::shared_ptr<VulkanDevice> m_Device;
		std::shared_ptr<VulkanSurface> m_Surface;
		std::shared_ptr<VulkanSwapChain> m_SwapChain;
    };
}