#pragma once

#include <vulkan/vulkan.h>
#include <VMA/vk_mem_alloc.h>

#include "Renderer/UniformBuffer.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(VulkanRenderContext& context, const void* data, uint32_t size);
		~VulkanUniformBuffer() override;

		void SetData(const void* data, uint32_t size) override;

		[[nodiscard]] VkBuffer GetVkBuffer() const { return m_VkBuffers[m_Context.GetSwapchain().GetFrameIndex()]; }
		[[nodiscard]] uint32_t GetSize() const { return m_Size; }

	private:
		VulkanRenderContext& m_Context;

		VkBuffer m_VkBuffers[FRAMES_IN_FLIGHT];
		VmaAllocation m_VmaAllocations[FRAMES_IN_FLIGHT];
		void* m_MappedData[FRAMES_IN_FLIGHT];

		uint32_t m_Size;

	public:
		friend class VulkanDescriptorSet;
	};
}