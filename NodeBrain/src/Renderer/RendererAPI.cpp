#include "NBpch.h"
#include "RendererAPI.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanRendererAPI.h"

namespace NodeBrain
{
	std::unique_ptr<RendererAPI> RendererAPI::Create()
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ASSERT(false, "Graphics API not detected!"); return nullptr;
			case GAPI::Vulkan: return std::make_unique<VulkanRendererAPI>();
		}
		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}