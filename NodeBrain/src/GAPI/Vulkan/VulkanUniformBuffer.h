#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanSwapchain.h"
#include "Renderer/UniformBuffer.h"

namespace NodeBrain
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(const void* data, uint32_t size);
		~VulkanUniformBuffer();

		virtual void SetData(const void* data, uint32_t size) override;

		virtual uint64_t GetAddress() const override { return m_VkDeviceAddresses[VulkanRenderContext::Get()->GetSwapchain().GetFrameIndex()]; }
		VkBuffer GetVkBuffer() const { return m_VkBuffers[VulkanRenderContext::Get()->GetSwapchain().GetFrameIndex()]; }
		uint32_t GetSize() const { return m_Size; }
	private:
		VkBuffer m_VkBuffers[FRAMES_IN_FLIGHT];
		VmaAllocation m_VmaAllocations[FRAMES_IN_FLIGHT];
		VkDeviceAddress m_VkDeviceAddresses[FRAMES_IN_FLIGHT];

		void* m_MappedData[FRAMES_IN_FLIGHT];

		uint32_t m_Size;
	};
}