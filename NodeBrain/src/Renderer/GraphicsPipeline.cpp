#include "NBpch.h"
#include "GraphicsPipeline.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"

namespace NodeBrain
{
    std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(std::shared_ptr<PipelineLayout> layout, std::shared_ptr<RenderPass> renderPass)
    {
        switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanGraphicsPipeline>(
				std::dynamic_pointer_cast<VulkanPipelineLayout>(layout), std::dynamic_pointer_cast<VulkanRenderPass>(renderPass));
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
    }
}