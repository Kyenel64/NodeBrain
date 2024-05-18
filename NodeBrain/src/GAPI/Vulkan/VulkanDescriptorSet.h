#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/DescriptorSet.h"

#include "GAPI/Vulkan/VulkanRenderContext.h"
#include "GAPI/Vulkan/VulkanSwapchain.h"

namespace NodeBrain
{
	class VulkanDescriptorSet : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(const std::vector<LayoutBinding>& layout);
		virtual ~VulkanDescriptorSet();

		virtual void WriteBuffer(std::shared_ptr<UniformBuffer> buffer, uint32_t binding) override;
		virtual void WriteImage(std::shared_ptr<Image> image, uint32_t binding) override;

		VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return m_VkDescriptorSetLayout; }
		VkDescriptorSet GetVkDescriptorSet() const { return m_VkDescriptorSet[VulkanRenderContext::Get()->GetSwapchain().GetFrameIndex()]; }

	private:
		VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet m_VkDescriptorSet[FRAMES_IN_FLIGHT];
	};
}