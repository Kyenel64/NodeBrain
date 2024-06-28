#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/DescriptorSet.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

namespace NodeBrain
{
	class VulkanDescriptorSet : public DescriptorSet
	{
	public:
		VulkanDescriptorSet(VulkanRenderContext& context, const std::vector<LayoutBinding>& layout);
		~VulkanDescriptorSet() override;

		void WriteBuffer(const std::shared_ptr<UniformBuffer>& buffer, uint32_t binding) override;
		void WriteImage(const std::shared_ptr<Image>& image, uint32_t binding) override;
		void WriteSampler(const std::shared_ptr<Image>& image, uint32_t binding) override;

		[[nodiscard]] VkDescriptorSetLayout GetVkDescriptorSetLayout() const { return m_VkDescriptorSetLayout; }
		[[nodiscard]] VkDescriptorSet GetVkDescriptorSet() const { return m_VkDescriptorSet[m_Context.GetSwapchain().GetFrameIndex()]; }

	private:
		VulkanRenderContext& m_Context;

		VkDescriptorSetLayout m_VkDescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet m_VkDescriptorSet[FRAMES_IN_FLIGHT];

		std::vector<LayoutBinding> m_Layout;
	};
}