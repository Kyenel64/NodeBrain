#include "NBpch.h"
#include "VulkanIndexBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* data, uint32_t size)
	{
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT; // Check
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_VkBuffer, &m_VmaAllocation, nullptr));

		void* gpuBuffer = nullptr;
		vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocation, &gpuBuffer);
		memcpy(gpuBuffer, data, size);
		vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocation);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_VkBuffer, m_VmaAllocation);
		m_VkBuffer = VK_NULL_HANDLE;
		m_VmaAllocation = VK_NULL_HANDLE;
	}

	void VulkanIndexBuffer::SetData(const void* data, uint32_t size)
	{
		void* gpuBuffer = nullptr;
		vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocation, &gpuBuffer);
		memcpy(gpuBuffer, data, size);
		vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocation);
	}
}
