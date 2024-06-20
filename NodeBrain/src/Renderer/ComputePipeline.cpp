#include "NBpch.h"
#include "ComputePipeline.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanComputePipeline.h"

namespace NodeBrain
{
	std::shared_ptr<ComputePipeline> ComputePipeline::Create(RenderContext& context, const ComputePipelineConfiguration& configuration)
	{
		switch (context.GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanComputePipeline>(dynamic_cast<VulkanRenderContext&>(context), configuration);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}