#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderPass.h"
#include "Renderer/PipelineLayout.h"
#include "Renderer/PipelineConfiguration.h"
#include "Renderer/GraphicsPipeline.h"

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
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();

	}

	
}