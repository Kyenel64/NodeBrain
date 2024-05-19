#include "NBpch.h"
#include "VulkanVertexBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, uint32_t size)
	{
		// --- GPU Buffer ---
		VkBufferCreateInfo gpuBufferCreateInfo = {};
		gpuBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		gpuBufferCreateInfo.size = size;
		gpuBufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocationCreateInfo.flags = 0;

		// --- CPU Buffer ---
		VkBufferCreateInfo cpuBufferCreateInfo = {};
		cpuBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		cpuBufferCreateInfo.size = size;
		cpuBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo cpuAllocationCreateInfo = {};
		cpuAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		cpuAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &gpuBufferCreateInfo, &allocationCreateInfo, &m_GPUBuffer[i], &m_GPUAllocation[i], nullptr));
			VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &cpuBufferCreateInfo, &cpuAllocationCreateInfo, &m_CPUBuffer[i], &m_CPUAllocation[i], nullptr));

			VkBufferDeviceAddressInfo deviceAddressInfo = {};
			deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			deviceAddressInfo.buffer = m_GPUBuffer[i];
			m_VkDeviceAddress[i] = vkGetBufferDeviceAddress(VulkanRenderContext::Get()->GetVkDevice(), &deviceAddressInfo);

			
			// Set initial data if provided
			if (data)
			{
				VulkanRenderContext::Get()->ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
				{
					void* cpuBuffer = nullptr;
					vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUAllocation[i], &cpuBuffer);
					memcpy(cpuBuffer, data, size);
					vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUAllocation[i]);

					memcpy(cpuBuffer, data, size);

					VkBufferCopy copy = {};
					copy.dstOffset = 0;
					copy.srcOffset = 0;
					copy.size = size;

					vkCmdCopyBuffer(cmdBuffer, m_CPUBuffer[i], m_GPUBuffer[i], 1, &copy);
				});
			}
		}
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_GPUBuffer[i], m_GPUAllocation[i]);
			m_GPUBuffer[i] = VK_NULL_HANDLE;
			m_GPUAllocation[i] = VK_NULL_HANDLE;

			vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUBuffer[i], m_CPUAllocation[i]);
			m_CPUBuffer[i] = VK_NULL_HANDLE;
			m_CPUAllocation[i] = VK_NULL_HANDLE;
		}
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			void* cpuBuffer = nullptr;
			vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUAllocation[i], &cpuBuffer);
			memcpy(cpuBuffer, data, size);
			vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_CPUAllocation[i]);

			VkBufferCopy copy = {};
			copy.dstOffset = 0;
			copy.srcOffset = 0;
			copy.size = size;
			vkCmdCopyBuffer(VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer, m_CPUBuffer[i], m_GPUBuffer[i], 1, &copy);
		}
	}
}
