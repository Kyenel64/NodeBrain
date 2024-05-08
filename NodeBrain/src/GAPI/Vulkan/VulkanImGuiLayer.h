#pragma once

#include <vulkan/vulkan.h>

#include "Core/ImGuiLayer.h"

namespace NodeBrain
{
	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer();
		virtual ~VulkanImGuiLayer();
		
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		virtual void BeginFrame() override;
		
		// Needs to be run within active command buffer and after copying draw image to swapchain. 
		virtual void EndFrame() override;
	private:
		VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;

		PFN_vkCmdBeginRenderingKHR m_vkCmdBeginRenderingKHR = VK_NULL_HANDLE;
		PFN_vkCmdEndRenderingKHR m_vkCmdEndRenderingKHR = VK_NULL_HANDLE;
	};
}