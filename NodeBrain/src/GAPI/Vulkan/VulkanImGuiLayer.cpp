#include "NBpch.h"
#include "VulkanImGuiLayer.h"

#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_vulkan.h"

namespace NodeBrain
{
	void VulkanImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
	}

	void VulkanImGuiLayer::OnDetach()
	{

	}

	void VulkanImGuiLayer::OnEvent(Event& e)
	{

	}

	void VulkanImGuiLayer::BeginFrame()
	{

	}

	void VulkanImGuiLayer::EndFrame()
	{

	}
}