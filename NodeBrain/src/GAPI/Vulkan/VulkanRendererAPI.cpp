#include "NBpch.h"
#include "VulkanRendererAPI.h"

#include "Core/App.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderPass.h"

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
	}

	VulkanRendererAPI::~VulkanRendererAPI()
	{
		NB_PROFILE_FN();

	}

	
}