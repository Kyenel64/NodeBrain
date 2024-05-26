#include "NBpch.h"
#include "RendererAPI.h"

#include "GAPI/Vulkan/VulkanRendererAPI.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	RendererAPI* RendererAPI::Create(RenderContext* renderContext)
	{
		switch (renderContext->GetGraphicsAPI())
		{
			case GAPI::None: NB_ASSERT(false, "Graphics API not detected!"); return nullptr;
			case GAPI::Vulkan: return new VulkanRendererAPI(dynamic_cast<VulkanRenderContext*>(renderContext));
		}
		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}