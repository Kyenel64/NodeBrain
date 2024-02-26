#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
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

		std::shared_ptr<Shader> vertShader = Shader::Create("rsc/Shaders/Compiled/triangle.vert.spv");
		std::shared_ptr<Shader> fragShader = Shader::Create("rsc/Shaders/Compiled/triangle.frag.spv");
		std::shared_ptr<GraphicsPipeline> pipeline = GraphicsPipeline::Create(vertShader, fragShader);
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();

	}

	
}