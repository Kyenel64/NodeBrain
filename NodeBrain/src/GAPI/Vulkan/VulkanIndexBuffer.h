#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Renderer/IndexBuffer.h"

namespace NodeBrain
{
	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* data, uint32_t size);
		virtual ~VulkanIndexBuffer();

		virtual void SetData(const void* data, uint32_t size) override;

		VkBuffer GetVkBuffer() const { return m_VkBuffer; }

	private:
		VkBuffer m_VkBuffer = VK_NULL_HANDLE;
		VmaAllocation m_VmaAllocation = VK_NULL_HANDLE;

		void* m_MappedData = nullptr;

		uint32_t m_Size;
	};
}
