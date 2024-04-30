#include "NBpch.h"
#include "VulkanVertexBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_VkBuffer, &m_VmaAllocation, nullptr));

		VkBufferDeviceAddressInfo deviceAddressInfo = {};
		deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		deviceAddressInfo.buffer = m_VkBuffer;

		m_VkDeviceAddress = vkGetBufferDeviceAddress(VulkanRenderContext::Get()->GetVkDevice(), &deviceAddressInfo);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_VkBuffer, m_VmaAllocation);
		m_VkBuffer = VK_NULL_HANDLE;
		m_VmaAllocation = VK_NULL_HANDLE;
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		void* gpuBuffer = nullptr;
		vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocation, &gpuBuffer);
		memcpy(gpuBuffer, data, size);
		vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocation);
	}
}
