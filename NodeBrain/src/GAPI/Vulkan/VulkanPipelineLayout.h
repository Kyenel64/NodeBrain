#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/PipelineLayout.h"
#include "GAPI/Vulkan/VulkanShader.h"
#include "GAPI/Vulkan/VulkanDevice.h"

namespace NodeBrain
{
	struct VulkanPipelineCreateInfos
	{
		VkPipelineShaderStageCreateInfo ShaderStages[2];
		
		VkPipelineVertexInputStateCreateInfo VertexInput;
		VkPipelineInputAssemblyStateCreateInfo InputAssembly;
		VkPipelineRasterizationStateCreateInfo Rasterizer;
		VkPipelineMultisampleStateCreateInfo Multisampling;
		VkPipelineDepthStencilStateCreateInfo DepthStencil;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment; 
		VkPipelineColorBlendStateCreateInfo ColorBlend;

		std::vector<VkDynamicState> DynamicStates;
		VkPipelineDynamicStateCreateInfo DynamicState;
		VkPipelineViewportStateCreateInfo ViewportState;

	};


	// TODO: Probably can put this together with GraphicsPipeline class. 
	class VulkanPipelineLayout : public PipelineLayout
	{
	public:
		VulkanPipelineLayout(const PipelineConfiguration& pipelineConfig);
		virtual ~VulkanPipelineLayout();

		const VulkanPipelineCreateInfos& GetCreateInfos() const { return m_CreateInfos; }
		VkPipelineLayout GetVkPipelineLayout() const { return m_VkPipelineLayout; }

	private:
		void Init();

	private:
		VkPipelineLayout m_VkPipelineLayout = VK_NULL_HANDLE;
		std::shared_ptr<VulkanDevice> m_Device;
		const PipelineConfiguration& m_PipelineConfig;
		VulkanPipelineCreateInfos m_CreateInfos = {};
	};
}