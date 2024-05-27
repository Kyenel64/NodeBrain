#include "NBpch.h"
#include "IndexBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanIndexBuffer.h"

namespace NodeBrain
{
	std::shared_ptr<IndexBuffer> IndexBuffer::Create(RenderContext* context, const uint32_t* data, uint32_t size)
	{
		switch (context->GetGraphicsAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanIndexBuffer>(dynamic_cast<VulkanRenderContext*>(context), data, size);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}