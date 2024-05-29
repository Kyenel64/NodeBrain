#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Renderer/IndexBuffer.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(VulkanRenderContext* context, const uint32_t* data, uint32_t size);
		~VulkanIndexBuffer() override;

		void SetData(const uint32_t* data, uint32_t size) override;

		VkBuffer GetVkBuffer() const { return m_VkBuffer; }

	private:
		VulkanRenderContext* m_Context;
		
		VkBuffer m_VkBuffer = VK_NULL_HANDLE;
		VmaAllocation m_VmaAllocation = VK_NULL_HANDLE;

		void* m_MappedData = nullptr;

		uint32_t m_Size;
	};
}
