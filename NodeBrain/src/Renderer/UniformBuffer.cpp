#include "NBpch.cpp"
#include "UniformBuffer.h"

#include "Renderer/Renderer.h"
#include "GAPI/Vulkan/VulkanUniformBuffer.h"

namespace NodeBrain
{
	std::shared_ptr<UniformBuffer> UniformBuffer::Create(const void* data, uint32_t size)
	{
		switch (Renderer::GetGAPI())
		{
			case GAPI::None: NB_ERROR("Graphics API not detected"); return nullptr;
			case GAPI::Vulkan: return std::make_shared<VulkanUniformBuffer>(data, size);
		}

		NB_ASSERT(false, "Graphics API not detected!");
		return nullptr;
	}
}