#include "NBpch.h"
#include "Framebuffer.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanFramebuffer.h"

namespace NodeBrain
{
	std::shared_ptr<Framebuffer> Framebuffer::Create(RenderContext& context, const FramebufferConfiguration& configuration)
	{
		switch (context.GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanFramebuffer>(dynamic_cast<VulkanRenderContext&>(context), configuration);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}