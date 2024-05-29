#pragma once

#include <vulkan/vulkan.h>

#include "Core/ImGuiLayer.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		explicit VulkanImGuiLayer(VulkanRenderContext* context);
		~VulkanImGuiLayer() override;
		
		void BeginFrame() override;
		
		// Needs to be run within active command buffer and after copying draw image to swapchain. 
		void EndFrame() override;
	private:
		VulkanRenderContext* m_Context;

		VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;

		PFN_vkCmdBeginRenderingKHR m_vkCmdBeginRenderingKHR = VK_NULL_HANDLE;
		PFN_vkCmdEndRenderingKHR m_vkCmdEndRenderingKHR = VK_NULL_HANDLE;
	};
}