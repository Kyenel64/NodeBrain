#include "NBpch.h"
#include "Renderer.h"

#include <glm/glm.hpp>

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

	struct ColorGradientData
	{
		glm::vec4 TopColor;
		glm::vec4 BottomColor;
		glm::vec4 Data3;
		glm::vec4 Data4;
	};

	struct RendererData
	{
		std::shared_ptr<Shader> TestVertexShader;
		std::shared_ptr<Shader> TestFragmentShader;
		std::shared_ptr<GraphicsPipeline> TestPipeline;

		std::shared_ptr<Shader> ColorGradientShader;
		std::shared_ptr<ComputePipeline> ColorGradientPipeline;
		ColorGradientData ColorGradientBuffer;

		std::shared_ptr<Shader> FlatColorShader;
		std::shared_ptr<ComputePipeline> FlatColorPipeline;
		glm::vec4 Color;
	};

	static RendererData* s_Data;

	GAPI Renderer::GetGAPI() { return s_GAPI; }

	void Renderer::Init()
	{
		NB_PROFILE_FN();

		s_Data = new RendererData();

		s_RendererAPI = RendererAPI::Create();
		NB_INFO("Initialized renderer");

		s_Data->TestVertexShader = Shader::Create("Assets/Shaders/Compiled/triangle.vert.spv", ShaderType::Vertex);
		s_Data->TestFragmentShader = Shader::Create("Assets/Shaders/Compiled/triangle.frag.spv", ShaderType::Fragment);
		PipelineConfiguration pipelineConfig = {};
		pipelineConfig.VertexShader = s_Data->TestVertexShader;
		pipelineConfig.FragmentShader = s_Data->TestFragmentShader;
		s_Data->TestPipeline = GraphicsPipeline::Create(pipelineConfig);

		// Color gradient shader.
		s_Data->ColorGradientShader = Shader::Create("Assets/Shaders/Compiled/gradientColor.comp.spv", ShaderType::Compute);
		// TODO: Setting layouts will most likely be done automatically by parsing shader file.
		s_Data->ColorGradientShader->SetLayout({ { BindingType::StorageImage, 1} });
		s_Data->ColorGradientShader->SetPushConstantLayout(sizeof(ColorGradientData), 0);
		s_Data->ColorGradientPipeline = ComputePipeline::Create(s_Data->ColorGradientShader);
		s_RendererAPI->TempUpdateImage(s_Data->ColorGradientShader);

		// Flat shader.
		s_Data->FlatColorShader = Shader::Create("Assets/Shaders/Compiled/flatColor.comp.spv", ShaderType::Compute);
		s_Data->FlatColorShader->SetLayout({ { BindingType::StorageImage, 1} });
		s_Data->FlatColorShader->SetPushConstantLayout(sizeof(glm::vec4), 64);
		s_Data->FlatColorPipeline = ComputePipeline::Create(s_Data->FlatColorShader);
		s_RendererAPI->TempUpdateImage(s_Data->FlatColorShader);
	}

	void Renderer::Shutdown()
	{
		NB_PROFILE_FN();

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

		s_Data->ColorGradientBuffer.TopColor = { 1, 0, 0, 1 };
		s_Data->ColorGradientBuffer.BottomColor = { 0, 0, 1, 1 };
		s_Data->ColorGradientPipeline->SetPushConstantData(&s_Data->ColorGradientBuffer, sizeof(ColorGradientData), 0);

		s_Data->Color = { 0, 1, 0, 1 };
		s_Data->FlatColorPipeline->SetPushConstantData(&s_Data->Color, sizeof(glm::vec4), 64);
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::ProcessGradientCompute()
	{
		s_RendererAPI->BeginComputePass(s_Data->ColorGradientPipeline);
		uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
		uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
		s_RendererAPI->DispatchCompute(groupX, groupY, 1);
		s_RendererAPI->EndComputePass();
	}
	
	void Renderer::ProcessFlatColorCompute()
	{
		s_RendererAPI->BeginComputePass(s_Data->FlatColorPipeline);
		uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
		uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
		s_RendererAPI->DispatchCompute(groupX, groupY, 1);
		s_RendererAPI->EndComputePass();
	}

	void Renderer::WaitForGPU()
	{
		s_RendererAPI->WaitForGPU();
	}

	void Renderer::BeginComputePass(std::shared_ptr<ComputePipeline> pipeline)
	{
		s_RendererAPI->BeginComputePass(pipeline);
	}

	void Renderer::EndComputePass()
	{
		s_RendererAPI->EndComputePass();
	}

	void Renderer::DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ)
	{
		s_RendererAPI->DispatchCompute(groupX, groupY, groupZ);
	}

	// Temp
	void Renderer::DrawGUI()
	{
		s_RendererAPI->DrawGUI();
	}

	void Renderer::TempUpdateImage(std::shared_ptr<Shader> shader)
	{
		s_RendererAPI->TempUpdateImage(shader);
	}

}