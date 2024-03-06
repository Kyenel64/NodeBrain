#include "NBpch.h"
#include "RenderPass.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanRenderPass.h"

namespace NodeBrain
{
    std::shared_ptr<RenderPass> RenderPass::Create()
    {
        switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanRenderPass>();
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
    }
}