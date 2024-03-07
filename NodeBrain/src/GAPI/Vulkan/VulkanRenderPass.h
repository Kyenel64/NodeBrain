#pragma once

#include "Renderer/RenderPass.h"
#include "GAPI/Vulkan/VulkanDevice.h"

namespace NodeBrain
{
    class VulkanRenderPass : public RenderPass
    {
    public:
        VulkanRenderPass();
        virtual ~VulkanRenderPass();

		VkRenderPass GetVkRenderPass() const { return m_VkRenderPass; }

    private:
		void Init();
    private:
		VkRenderPass m_VkRenderPass;
        std::shared_ptr<VulkanDevice> m_Device;

    };
}