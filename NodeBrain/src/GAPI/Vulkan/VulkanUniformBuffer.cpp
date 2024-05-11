#include "NBpch.h"
#include "VulkanUniformBuffer.h"

namespace NodeBrain
{
	VulkanUniformBuffer::VulkanUniformBuffer(const void* data, uint32_t size)
		: m_Size(size)
	{
		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocationCreateInfo.flags = 0;

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			VK_CHECK(vmaCreateBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_VkBuffers[i], &m_VmaAllocations[i], nullptr));
			VK_CHECK(vmaMapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocations[i], &m_MappedData[i]));

			VkBufferDeviceAddressInfo deviceAddressInfo = {};
			deviceAddressInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
			deviceAddressInfo.buffer = m_VkBuffers[i];

			m_VkDeviceAddresses[i] = vkGetBufferDeviceAddress(VulkanRenderContext::Get()->GetVkDevice(), &deviceAddressInfo);
		}


		// Set initial data if provided
		if (data)
		{
			for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
				memcpy(m_MappedData[VulkanRenderContext::Get()->GetSwapchain().GetFrameIndex()], data, size);
		}
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			vmaUnmapMemory(VulkanRenderContext::Get()->GetVMAAllocator(), m_VmaAllocations[i]);
			vmaDestroyBuffer(VulkanRenderContext::Get()->GetVMAAllocator(), m_VkBuffers[i], m_VmaAllocations[i]);
			m_VkBuffers[i] = VK_NULL_HANDLE;
			m_VmaAllocations[i] = VK_NULL_HANDLE;
		}
	}

	void VulkanUniformBuffer::SetData(const void* data, uint32_t size)
	{
		m_Size = size;
		memcpy(m_MappedData[VulkanRenderContext::Get()->GetSwapchain().GetFrameIndex()], data, size);
	}
}