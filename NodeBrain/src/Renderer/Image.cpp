#include "NBpch.h"
#include "Image.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanImage.h"

namespace NodeBrain
{
	std::shared_ptr<Image> Image::Create(const ImageConfiguration& configuration)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanImage>(configuration);
		}
		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}