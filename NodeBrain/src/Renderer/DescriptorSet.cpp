#include "NBpch.h"
#include "DescriptorSet.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanDescriptorSet.h"

namespace NodeBrain
{
	std::shared_ptr<DescriptorSet> DescriptorSet::Create(RenderContext* context, const std::vector<LayoutBinding>& layout)
	{
		switch (context->GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanDescriptorSet>(dynamic_cast<VulkanRenderContext*>(context), layout);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}