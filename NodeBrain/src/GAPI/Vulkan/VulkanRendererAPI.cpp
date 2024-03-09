#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderPass.h"
#include "Renderer/PipelineLayout.h"
#include "Renderer/PipelineConfiguration.h"
#include "Renderer/GraphicsPipeline.h"

// temp
#include "GAPI/Vulkan/VulkanFramebuffer.h"
#include "GAPI/Vulkan/VulkanSwapChain.h"
#include "GAPI/Vulkan/VulkanRenderContext.h"

#define NB_LIST_AVAILABLE_VK_EXTENSTIONS

namespace NodeBrain
{
	VulkanRendererAPI::VulkanRendererAPI()
	{
		NB_PROFILE_FN();

		Init();
	}

	void VulkanRendererAPI::Init()
	{
		NB_PROFILE_FN();

		std::shared_ptr<Shader> vertShader = Shader::Create("Assets/Shaders/Compiled/triangle.vert.spv");
		std::shared_ptr<Shader> fragShader = Shader::Create("Assets/Shaders/Compiled/triangle.frag.spv");
		std::shared_ptr<RenderPass> renderPass = RenderPass::Create();
		PipelineConfiguration pipelineConfig = {};
		pipelineConfig.VertexShader = vertShader;
		pipelineConfig.FragmentShader = fragShader;
		std::shared_ptr<PipelineLayout> pipelineLayout = PipelineLayout::Create(pipelineConfig);
		std::shared_ptr<GraphicsPipeline> pipeline = GraphicsPipeline::Create(pipelineLayout, renderPass);
		
		// temp
		std::shared_ptr<VulkanSwapChain> swapchain = VulkanRenderContext::GetInstance()->GetSwapchain();
		std::vector<std::shared_ptr<VulkanFramebuffer>> framebuffers;
		for (int i = 0; i < swapchain->GetSwapchainImages().size(); i++)
			framebuffers.push_back(std::make_shared<VulkanFramebuffer>(renderPass, swapchain->GetSwapchainImages()[i]->GetVkImageView()));

	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();

	}

	
}