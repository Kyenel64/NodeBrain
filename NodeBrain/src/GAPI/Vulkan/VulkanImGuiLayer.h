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
		virtual void EndFrame() override;
	private:
		VkDescriptorPool m_ImGuiDescriptorPool = VK_NULL_HANDLE;
	};
}