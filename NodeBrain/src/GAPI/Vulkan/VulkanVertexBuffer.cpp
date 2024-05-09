#include "NBpch.h"
#include "VulkanVertexBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, uint32_t size)
	{
		// --- GPU Buffer ---
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocationCreateInfo.flags = 0;

		VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_GPUBuffer, &m_GPUAllocation, nullptr));

		VkBufferDeviceAddressInfo deviceAddressInfo = {};
		deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		deviceAddressInfo.buffer = m_GPUBuffer;

		m_VkDeviceAddress = vkGetBufferDeviceAddress(VulkanRenderContext::Get()->GetVkDevice(), &deviceAddressInfo);

		
		// --- CPU Buffer ---
		VkBufferCreateInfo cpuBufferCreateInfo = {};
		cpuBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		cpuBufferCreateInfo.size = size;
		cpuBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo cpuAllocationCreateInfo = {};
		cpuAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		cpuAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &cpuBufferCreateInfo, &cpuAllocationCreateInfo, &m_CPUBuffer, &m_CPUAllocation, nullptr));

		
		// Set initial data if provided
		if (data)
		{
			VulkanRenderContext::Get()->ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
			{
				void* cpuBuffer = nullptr;
				vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUAllocation, &cpuBuffer);
				memcpy(cpuBuffer, data, size);
				vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUAllocation);

				memcpy(cpuBuffer, data, size);

				VkBufferCopy copy = {};
				copy.dstOffset = 0;
				copy.srcOffset = 0;
				copy.size = size;

				vkCmdCopyBuffer(cmdBuffer, m_CPUBuffer, m_GPUBuffer, 1, &copy);
			});
		}
		
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_GPUBuffer, m_GPUAllocation);
		m_GPUBuffer = VK_NULL_HANDLE;
		m_GPUAllocation = VK_NULL_HANDLE;

		vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUBuffer, m_CPUAllocation);
		m_CPUBuffer = VK_NULL_HANDLE;
		m_CPUAllocation = VK_NULL_HANDLE;
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		void* cpuBuffer = nullptr;
		vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUAllocation, &cpuBuffer);
		memcpy(cpuBuffer, data, size);
		vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUAllocation);

		memcpy(cpuBuffer, data, size);

		VkBufferCopy copy = {};
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = size;

		vkCmdCopyBuffer(VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer, m_CPUBuffer, m_GPUBuffer, 1, &copy);
	}
}
