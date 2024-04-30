#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Renderer/VertexBuffer.h"

namespace NodeBrain
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(uint32_t size);
		virtual ~VulkanVertexBuffer();

		virtual void SetData(const void* data, uint32_t size) override;
		
		virtual uint64_t GetAddress() const override { return m_VkDeviceAddress; }
		VkBuffer GetVkBuffer() const { return m_VkBuffer; }

	private:
		VkBuffer m_VkBuffer = VK_NULL_HANDLE;
		VmaAllocation m_VmaAllocation = VK_NULL_HANDLE;
		VkDeviceAddress m_VkDeviceAddress;
	};
}
