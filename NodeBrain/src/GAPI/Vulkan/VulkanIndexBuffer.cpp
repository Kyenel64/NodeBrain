#include "NBpch.h"
#include "VulkanIndexBuffer.h"

namespace NodeBrain
{
	VulkanIndexBuffer::VulkanIndexBuffer(VulkanRenderContext* context, const uint32_t* data, uint32_t size)
		: m_Context(context), m_Size(size)
	{
		NB_PROFILE_FN();

		NB_ASSERT(context, "context null. A valid VulkanRenderContext pointer is required to create VulkanIndexBuffer.");
		NB_ASSERT(size, "size is 0. Size must be a non-zero value in bytes.");

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; // Check
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VK_CHECK(vmaCreateBuffer(m_Context->GetVMAAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_VkBuffer, &m_VmaAllocation, nullptr));
		VK_CHECK(vmaMapMemory(m_Context->GetVMAAllocator(), m_VmaAllocation, &m_MappedData));

		// Set initial data if provided
		if (data)
			memcpy(m_MappedData, data, size);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		NB_PROFILE_FN();

		m_Context->WaitForGPU();

		vmaUnmapMemory(m_Context->GetVMAAllocator(), m_VmaAllocation);
		vmaDestroyBuffer(m_Context->GetVMAAllocator(), m_VkBuffer, m_VmaAllocation);
		m_VkBuffer = VK_NULL_HANDLE;
		m_VmaAllocation = VK_NULL_HANDLE;
	}

	void VulkanIndexBuffer::SetData(const uint32_t* data, uint32_t size)
	{
		NB_PROFILE_FN();

		NB_ASSERT(data, "Invalid data. Provided data must not be null.");
		NB_ASSERT(size <= m_Size, "Buffer overflow. The size of data being set must be less than the allocated buffer size.");
		
		memcpy(m_MappedData, data, size);
	}
}
