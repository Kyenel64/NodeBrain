#include "NBpch.h"
#include "VulkanVertexBuffer.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	VulkanVertexBuffer::VulkanVertexBuffer(const void* data, uint32_t size)
	{
		NB_PROFILE_FN();

		// --- GPU Buffer ---
		VkBufferCreateInfo gpuBufferCreateInfo = {};
		gpuBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		gpuBufferCreateInfo.size = size;
		gpuBufferCreateInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

		VmaAllocationCreateInfo gpuAllocationCreateInfo = {};
		gpuAllocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		gpuAllocationCreateInfo.flags = 0;

		// --- Staging Buffer ---
		VkBufferCreateInfo stagingBufferCreateInfo = {};
		stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferCreateInfo.size = size;
		stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo stagingAllocationCreateInfo = {};
		stagingAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		stagingAllocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &gpuBufferCreateInfo, &gpuAllocationCreateInfo, &m_GPUBuffer[i], &m_GPUAllocation[i], nullptr));
			VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &stagingBufferCreateInfo, &stagingAllocationCreateInfo, &m_StagingBuffer[i], &m_StagingAllocation[i], nullptr));

			VkBufferDeviceAddressInfo deviceAddressInfo = {};
			deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			deviceAddressInfo.buffer = m_GPUBuffer[i];
			m_VkDeviceAddress[i] = vkGetBufferDeviceAddress(VulkanRenderContext::Get()->GetVkDevice(), &deviceAddressInfo);

			VK_CHECK(vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_StagingAllocation[i], &m_StagingMappedData[i]));

			// Set initial data if provided
			if (data)
			{
				VulkanRenderContext::Get()->ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
				{
					memcpy(m_StagingMappedData[i], data, size);

					VkBufferCopy copy = {};
					copy.dstOffset = 0;
					copy.srcOffset = 0;
					copy.size = size;
					vkCmdCopyBuffer(cmdBuffer, m_StagingBuffer[i], m_GPUBuffer[i], 1, &copy);
				});
			}
		}
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		NB_PROFILE_FN();

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_StagingAllocation[i]);
			
			vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_GPUBuffer[i], m_GPUAllocation[i]);
			m_GPUBuffer[i] = VK_NULL_HANDLE;
			m_GPUAllocation[i] = VK_NULL_HANDLE;

			vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_StagingBuffer[i], m_StagingAllocation[i]);
			m_StagingBuffer[i] = VK_NULL_HANDLE;
			m_StagingAllocation[i] = VK_NULL_HANDLE;
		}
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
		NB_PROFILE_FN();
		
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			memcpy(m_StagingMappedData[i], data, size);

			VkBufferCopy copy = {};
			copy.dstOffset = 0;
			copy.srcOffset = 0;
			copy.size = size;
			vkCmdCopyBuffer(VulkanRenderContext::Get()->GetSwapchain().GetCurrentFrameData().CommandBuffer, m_StagingBuffer[i], m_GPUBuffer[i], 1, &copy);
		}
	}
}
