#include "NBpch.h"
#include "IndexBuffer.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanIndexBuffer.h"

namespace NodeBrain
{
	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* data, uint32_t size)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanIndexBuffer>(data, size);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}