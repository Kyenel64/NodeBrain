#include "NBpch.h"
#include "Image.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanImage.h"

namespace NodeBrain
{
	std::shared_ptr<Image> Image::Create(RenderContext& context, const ImageConfiguration& configuration)
	{
		switch (context.GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanImage>(dynamic_cast<VulkanRenderContext&>(context), configuration);
		}
		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}