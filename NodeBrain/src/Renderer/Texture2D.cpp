#include "NBpch.h"
#include "Texture2D.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanTexture2D.h"

namespace NodeBrain
{
	std::shared_ptr<Texture2D> Texture2D::Create(RenderContext& context, const Texture2DConfiguration& configuration)
	{
		switch (context.GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanTexture2D>(dynamic_cast<VulkanRenderContext&>(context), configuration);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}

	std::shared_ptr<Texture2D> Texture2D::Create(RenderContext& context, std::filesystem::path path)
	{
		switch (context.GetGraphicsAPI())
		{
		case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
		case GAPI::Vulkan: return std::make_shared<VulkanTexture2D>(dynamic_cast<VulkanRenderContext&>(context), std::move(path));
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}
