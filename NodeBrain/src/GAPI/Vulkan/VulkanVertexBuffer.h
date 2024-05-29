#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Renderer/VertexBuffer.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(VulkanRenderContext* context, const void* data, uint32_t size);
		~VulkanVertexBuffer() override;

		void SetData(const void* data, uint32_t size) override;
		
		uint64_t GetAddress() const override { return m_VkDeviceAddress[m_Context->GetSwapchain().GetFrameIndex()]; }
		VkBuffer GetVkBuffer() const { return m_StagingBuffer[m_Context->GetSwapchain().GetFrameIndex()]; }

	private:
		VulkanRenderContext* m_Context;

		VkBuffer m_StagingBuffer[FRAMES_IN_FLIGHT];
		VkBuffer m_GPUBuffer[FRAMES_IN_FLIGHT];
		VmaAllocation m_StagingAllocation[FRAMES_IN_FLIGHT];
		VmaAllocation m_GPUAllocation[FRAMES_IN_FLIGHT];
		VkDeviceAddress m_VkDeviceAddress[FRAMES_IN_FLIGHT];
		void* m_StagingMappedData[FRAMES_IN_FLIGHT];

		uint32_t m_Size;
	};
}
