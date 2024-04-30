#include "NBpch.h"
#include "VertexBuffer.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanVertexBuffer.h"

namespace NodeBrain
{
	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanVertexBuffer>(size);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}