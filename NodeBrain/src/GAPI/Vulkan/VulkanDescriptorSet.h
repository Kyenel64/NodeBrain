#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/DescriptorSet.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanDescriptorSet : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(VulkanRenderContext* context, const std::vector<LayoutBinding>& layout);
		~VulkanDescriptorSet() override;

		virtual void WriteBuffer(std::shared_ptr<UniformBuffer> buffer, uint32_t binding) override;
		virtual void WriteImage(std::shared_ptr<Image> image, uint32_t binding) override;

		VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return m_VkDescriptorSetLayout; }
		VkDescriptorSet GetVkDescriptorSet() const { return m_VkDescriptorSet[m_Context->GetSwapchain().GetFrameIndex()]; }

	private:
		VulkanRenderContext* m_Context;

		VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet m_VkDescriptorSet[FRAMES_IN_FLIGHT];

		std::vector<LayoutBinding> m_Layout;
	};
}