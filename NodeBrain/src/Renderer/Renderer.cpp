#include "NBpch.h"
#include "Renderer.h"

#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/ComputePipeline.h"
#include "RenderContext.h"
#include "Core/App.h"

namespace NodeBrain
{
	std::unique_ptr<RendererAPI> s_RendererAPI = nullptr;
	GAPI s_GAPI = GAPI::Vulkan;

	struct RendererData
	{
		std::shared_ptr<Shader> TestVertexShader;
		std::shared_ptr<Shader> TestFragmentShader;
		std::shared_ptr<GraphicsPipeline> TestPipeline;

		std::shared_ptr<Shader> TestComputeShader;
		std::shared_ptr<ComputePipeline> TestComputePipeline;

		//std::shared_ptr<Texture2D> TestTexture;
	};

	static RendererData* s_Data;

	void Renderer::Init()
	{
		NB_PROFILE_FN();

		s_Data = new RendererData();

		s_RendererAPI = RendererAPI::Create();
		NB_INFO("Initialized renderer");

		s_Data->TestVertexShader = Shader::Create("Assets/Shaders/Compiled/triangle.vert.spv");
		s_Data->TestFragmentShader = Shader::Create("Assets/Shaders/Compiled/triangle.frag.spv");
		PipelineConfiguration pipelineConfig = {};
		pipelineConfig.VertexShader = s_Data->TestVertexShader;
		pipelineConfig.FragmentShader = s_Data->TestFragmentShader;
		s_Data->TestPipeline = GraphicsPipeline::Create(pipelineConfig);


		// Compute shader
		s_Data->TestComputeShader = Shader::Create("Assets/Shaders/Compiled/gradient.comp.spv"); // Set descriptor layout
		s_Data->TestComputePipeline = ComputePipeline::Create(s_Data->TestComputeShader);

		s_RendererAPI->TempUpdateImage(s_Data->TestComputeShader);
	}

	void Renderer::Shutdown()
	{
		NB_PROFILE_FN();

		s_RendererAPI->WaitForGPU();

		delete s_Data;

		s_RendererAPI.reset();
		NB_INFO("Shutdown renderer");
	}

	void Renderer::BeginFrame()
	{
		s_RendererAPI->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		s_RendererAPI->EndFrame();
	}

	void Renderer::BeginScene()
	{
		s_RendererAPI->ClearColor({ 0.3f, 0.3f, 0.8f, 1.0f });
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::DrawTestTriangle()
	{
		//s_Data->TestTexture->Bind();
		//s_RendererAPI->DrawTestTriangle(s_Data->TestPipeline);
	}

	void Renderer::ProcessTestCompute() //std::shared_ptr<Texture2D> targetTexture
	{
		s_RendererAPI->BeginComputePass(s_Data->TestComputePipeline);
		uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
		uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
		s_RendererAPI->DispatchCompute(groupX, groupY, 1);
		s_RendererAPI->EndComputePass();
	}

	void Renderer::DrawGUI()
	{
		s_RendererAPI->DrawGUI();
	}

	GAPI Renderer::GetGAPI() { return s_GAPI; }
}