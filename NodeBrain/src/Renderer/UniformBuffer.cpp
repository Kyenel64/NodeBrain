#include "NBpch.cpp"
#include "UniformBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanUniformBuffer.h"

namespace NodeBrain
{
	std::shared_ptr<UniformBuffer> UniformBuffer::Create(RenderContext& context, const void* data, uint32_t size)
	{
		switch (context.GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanUniformBuffer>(dynamic_cast<VulkanRenderContext&>(context), data, size);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}