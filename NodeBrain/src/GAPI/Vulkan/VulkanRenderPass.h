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

    private:
		void Init();
    private:
		VkRenderPass m_VkRenderPass;
        std::shared_ptr<VulkanDevice> m_Device;

    };
}