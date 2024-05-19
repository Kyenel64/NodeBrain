#include "NBpch.h"
#include "VulkanIndexBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* data, uint32_t size)
		: m_Size(size)
	{
		NB_PROFILE_FN();

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; // Check
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_VkBuffer, &m_VmaAllocation, nullptr));
		VK_CHECK(vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocation, &m_MappedData));

		// Set initial data if provided
		if (data)
			memcpy(m_MappedData, data, size);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		NB_PROFILE_FN();

		vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocation);
		vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_VkBuffer, m_VmaAllocation);
		m_VkBuffer = VK_NULL_HANDLE;
		m_VmaAllocation = VK_NULL_HANDLE;
	}

	void VulkanIndexBuffer::SetData(const void* data, uint32_t size)
	{
		NB_PROFILE_FN();

		NB_ASSERT(data, "Invalid data. Provided data must not be null.");
		NB_ASSERT(size <= m_Size, "Buffer overflow. The size of data being set must be less than the allocated buffer size.");
		
		memcpy(m_MappedData, data, size);
	}
}
