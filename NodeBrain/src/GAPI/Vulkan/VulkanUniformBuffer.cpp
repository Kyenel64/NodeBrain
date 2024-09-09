#include "NBpch.h"
#include "VulkanUniformBuffer.h"

namespace NodeBrain
{
	VulkanUniformBuffer::VulkanUniformBuffer(VulkanRenderContext& context, const void* data, uint32_t size)
		: m_Context(context), m_Size(size)
	{
		NB_PROFILE_FN();

		NB_ASSERT(size, "size is 0. Size must be a non-zero value in bytes.");

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocationCreateInfo.flags = 0;

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VK_CHECK(vmaCreateBuffer(m_Context.GetVMAAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_VkBuffers[i], &m_VmaAllocations[i], nullptr));
			VK_CHECK(vmaMapMemory(m_Context.GetVMAAllocator(), m_VmaAllocations[i], &m_MappedData[i]));
		}

		// Set initial data if provided
		if (data)
		{
			for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
				memcpy(m_MappedData[m_Context.GetSwapchain().GetFrameIndex()], data, size);
		}
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		NB_PROFILE_FN();

		m_Context.WaitForGPU();

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vmaUnmapMemory(m_Context.GetVMAAllocator(), m_VmaAllocations[i]);
			vmaDestroyBuffer(m_Context.GetVMAAllocator(), m_VkBuffers[i], m_VmaAllocations[i]);
			m_VkBuffers[i] = VK_NULL_HANDLE;
			m_VmaAllocations[i] = VK_NULL_HANDLE;
		}
	}

	void VulkanUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		NB_PROFILE_FN();

		NB_ASSERT(data, "data null. Data must not be null.");
		NB_ASSERT(size + offset <= m_Size, "Buffer overflow. Size + offset must not exceed the size of the allocated buffer.");

		if (m_Context.IsInRuntime())
		{
			memcpy((uint8_t*)m_MappedData[m_Context.GetSwapchain().GetFrameIndex()] + offset, data, size);
		}
		else
		{
			m_Context.ImmediateSubmit([&](VkCommandBuffer cmdBuffer)
			{
				for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
					memcpy((uint8_t*)m_MappedData[i] + offset, data, size);
			});
		}
	}
}
