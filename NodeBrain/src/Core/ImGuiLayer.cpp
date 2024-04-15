#include "NBpch.h"
#include "ImGuiLayer.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanImGuiLayer.h"

namespace NodeBrain
{
    ImGuiLayer* ImGuiLayer::Create()
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return new VulkanImGuiLayer();
		}
		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}