#include "NBpch.h"
#include "DescriptorSet.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanDescriptorSet.h"

namespace NodeBrain
{
	std::shared_ptr<DescriptorSet> DescriptorSet::Create(const std::vector<LayoutBinding>& layout)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanDescriptorSet>(layout);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}