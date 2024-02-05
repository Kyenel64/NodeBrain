#include "NBpch.h"
#include "RenderContext.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	std::unique_ptr<RenderContext> RenderContext::Create(Window* window)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_unique<VulkanRenderContext>(window);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}