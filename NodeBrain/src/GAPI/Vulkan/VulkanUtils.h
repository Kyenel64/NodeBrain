#pragma once

#include <vulkan/vk_enum_string_helper.h>

#include "Renderer/Image.h"

namespace NodeBrain
{
	#define VK_CHECK(result) if (result) NB_ASSERT(false, string_VkResult(result))

	VkFormat ImageFormatToVulkanFormat(ImageFormat format);
}