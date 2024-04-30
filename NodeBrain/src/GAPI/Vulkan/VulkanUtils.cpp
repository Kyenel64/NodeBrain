#include "NBpch.h"
#include "VulkanUtils.h"

#include "GAPI/Vulkan/VulkanImage.h"

namespace NodeBrain
{
    VkFormat ImageFormatToVkFormat(ImageFormat format)
	{
		switch (format)
		{
			case ImageFormat::None: return VK_FORMAT_UNDEFINED; break;
			case ImageFormat::RGBA16: return VK_FORMAT_R16G16B16A16_SFLOAT; break;
			case ImageFormat::RGBA8: return VK_FORMAT_B8G8R8A8_SRGB; break;
		}

		return VK_FORMAT_UNDEFINED;
	}
}