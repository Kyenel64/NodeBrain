#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "GAPI/Vulkan/VulkanSwapchain.h"
#include "Renderer/VertexBuffer.h"

namespace NodeBrain
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const void* data, uint32_t size);
		virtual ~VulkanVertexBuffer();

		virtual void SetData(const void* data, uint32_t size) override;
		
		virtual uint64_t GetAddress() const override { return m_VkDeviceAddress[VulkanRenderContext::Get()->GetSwapchain().GetFrameIndex()]; }
		VkBuffer GetVkBuffer() const { return m_StagingBuffer[VulkanRenderContext::Get()->GetSwapchain().GetFrameIndex()]; }

	private:
		VkBuffer m_StagingBuffer[FRAMES_IN_FLIGHT];
		VkBuffer m_GPUBuffer[FRAMES_IN_FLIGHT];
		VmaAllocation m_StagingAllocation[FRAMES_IN_FLIGHT];
		VmaAllocation m_GPUAllocation[FRAMES_IN_FLIGHT];
		VkDeviceAddress m_VkDeviceAddress[FRAMES_IN_FLIGHT];
		void* m_StagingMappedData[FRAMES_IN_FLIGHT];
	};
}
