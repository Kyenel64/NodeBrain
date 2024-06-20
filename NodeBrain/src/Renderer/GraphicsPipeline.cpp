#include "NBpch.h"
#include "GraphicsPipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanGraphicsPipeline.h"

namespace NodeBrain
{
	std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(RenderContext& context, const GraphicsPipelineConfiguration& configuration)
	{
		switch (context.GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanGraphicsPipeline>(dynamic_cast<VulkanRenderContext&>(context), configuration);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}