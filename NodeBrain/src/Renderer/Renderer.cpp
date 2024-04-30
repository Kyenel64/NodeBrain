#include "NBpch.h"
#include "Renderer.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"
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

	struct QuadVertex
	{
		glm::vec3 Position;
		float UVX;
		glm::vec3 Normal;
		float UVY;
		glm::vec4 Color;
	};

	struct PushConstantData
	{
		glm::mat4 ViewMatrix;
		uint64_t Address;
	};

	struct RendererData
	{
		std::shared_ptr<Shader> TestVertexShader;
		std::shared_ptr<Shader> TestFragmentShader;
		std::shared_ptr<GraphicsPipeline> TestPipeline;

		QuadVertex TestVertexData[4];
		std::shared_ptr<VertexBuffer> TestVertexBuffer;
		std::shared_ptr<IndexBuffer> TestIndexBuffer;
		PushConstantData TestPushConstantData;

		std::shared_ptr<Shader> ColorGradientShader;
		std::shared_ptr<ComputePipeline> ColorGradientPipeline;
		ColorGradientData ColorGradientBuffer;

		std::shared_ptr<Shader> FlatColorShader;
		std::shared_ptr<ComputePipeline> FlatColorPipeline;
		glm::vec4 Color;
	};

	static RendererData* s_Data = nullptr;

	GAPI Renderer::GetGAPI() { return s_GAPI; }

	void Renderer::Init()
	{
		NB_PROFILE_FN();

		s_Data = new RendererData();

		s_RendererAPI = RendererAPI::Create();
		NB_INFO("Initialized renderer");

		s_Data->TestVertexShader = Shader::Create("Assets/Shaders/Compiled/triangle.vert.spv", ShaderType::Vertex);
		s_Data->TestVertexShader->SetPushConstantLayout(sizeof(PushConstantData), 0);
		s_Data->TestFragmentShader = Shader::Create("Assets/Shaders/Compiled/triangle.frag.spv", ShaderType::Fragment);
		PipelineConfiguration pipelineConfig = {};
		pipelineConfig.VertexShader = s_Data->TestVertexShader;
		pipelineConfig.FragmentShader = s_Data->TestFragmentShader;
		s_Data->TestPipeline = GraphicsPipeline::Create(pipelineConfig);

		s_Data->TestVertexData[0].Position = {  0.5f, -0.5f, 0.0f };
		s_Data->TestVertexData[1].Position = {  0.5f,  0.5f, 0.0f };
		s_Data->TestVertexData[2].Position = { -0.5f, -0.5f, 0.0f };
		s_Data->TestVertexData[3].Position = { -0.5f,  0.5f, 0.0f };

		s_Data->TestVertexData[0].Color = { 0.0f, 0.0f, 0.0f, 1.0f };
		s_Data->TestVertexData[1].Color = { 0.5f, 0.5f, 0.5f, 1.0f };
		s_Data->TestVertexData[2].Color = { 1.0f, 0.0f, 0.0f, 1.0f };
		s_Data->TestVertexData[3].Color = { 0.0f, 1.0f, 0.0f, 1.0f };

		s_Data->TestVertexBuffer = VertexBuffer::Create(sizeof(QuadVertex) * 4);

		uint32_t* quadIndices = new uint32_t[6];
		quadIndices[0] = 0;
		quadIndices[1] = 1;
		quadIndices[2] = 2;
		quadIndices[3] = 2;
		quadIndices[4] = 1;
		quadIndices[5] = 3;

		s_Data->TestIndexBuffer = IndexBuffer::Create(quadIndices, sizeof(uint32_t) * 6);
		delete[] quadIndices;



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
		//s_RendererAPI->ClearColor({ 0.3f, 0.3f, 0.8f, 1.0f });
	}

	void Renderer::EndScene()
	{
		s_Data->TestPushConstantData.ViewMatrix = glm::mat4(1.0f);
		s_Data->TestPushConstantData.Address = s_Data->TestVertexBuffer->GetAddress();
		s_Data->TestPipeline->SetPushConstantData(&s_Data->TestPushConstantData, sizeof(PushConstantData), 0);
		float y = sin(glfwGetTime()) * 1.0f;
		s_Data->TestVertexData[0].Position = { y, -y, 0.0f };
		s_Data->TestVertexBuffer->SetData(s_Data->TestVertexData, sizeof(QuadVertex) * 4);
		s_RendererAPI->BeginRenderPass();
		s_RendererAPI->BindGraphicsPipeline(s_Data->TestPipeline);
		s_RendererAPI->BindIndexBuffer(s_Data->TestIndexBuffer);
		s_RendererAPI->DrawIndexed(6, 0); // temp
		s_RendererAPI->EndRenderPass();
	}

	// Backend
	void Renderer::WaitForGPU()
	{
		s_RendererAPI->WaitForGPU();
	}

	void Renderer::BeginRenderPass()
	{
		s_RendererAPI->BeginRenderPass();
	}

	void Renderer::EndRenderPass()
	{
		s_RendererAPI->EndRenderPass();
	}

	void Renderer::Draw(uint32_t vertexCount, uint32_t vertexIndex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		s_RendererAPI->Draw(vertexCount, vertexIndex, instanceCount, instanceIndex);
	}

	void Renderer::DrawIndexed(uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		s_RendererAPI->DrawIndexed(indexCount, firstIndex, instanceCount, instanceIndex);
	}

	void Renderer::BindGraphicsPipeline(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		s_RendererAPI->BindGraphicsPipeline(pipeline);
	}

	void Renderer::BindIndexBuffer(std::shared_ptr<IndexBuffer> indexBuffer)
	{
		s_RendererAPI->BindIndexBuffer(indexBuffer);
	}

	void Renderer::BeginComputePass()
	{
		s_RendererAPI->BeginComputePass();
	}

	void Renderer::EndComputePass()
	{
		s_RendererAPI->EndComputePass();
	}

	void Renderer::BindComputePipeline(std::shared_ptr<ComputePipeline> pipeline)
	{
		s_RendererAPI->BindComputePipeline(pipeline);
	}

	void Renderer::DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ)
	{
		s_RendererAPI->DispatchCompute(groupX, groupY, groupZ);
	}


	// Temp
	void Renderer::TempUpdateImage(std::shared_ptr<Shader> shader)
	{
		s_RendererAPI->TempUpdateImage(shader);
	}

	void Renderer::ProcessGradientCompute()
	{
		s_Data->ColorGradientBuffer.TopColor = { 1, 0, 0, 1 };
		s_Data->ColorGradientBuffer.BottomColor = { 0, 0, 1, 1 };
		s_Data->ColorGradientPipeline->SetPushConstantData(&s_Data->ColorGradientBuffer, sizeof(ColorGradientData), 0);

		s_RendererAPI->BeginComputePass();
		s_RendererAPI->BindComputePipeline(s_Data->ColorGradientPipeline);
		uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
		uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
		s_RendererAPI->DispatchCompute(groupX, groupY, 1);
		s_RendererAPI->EndComputePass();
	}
	
	void Renderer::ProcessFlatColorCompute()
	{
		s_Data->Color = { 0, 1, 0, 1 };
		s_Data->FlatColorPipeline->SetPushConstantData(&s_Data->Color, sizeof(glm::vec4), 64);

		s_RendererAPI->BeginComputePass();
		s_RendererAPI->BindComputePipeline(s_Data->FlatColorPipeline);
		uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
		uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
		s_RendererAPI->DispatchCompute(groupX, groupY, 1);
		s_RendererAPI->EndComputePass();
	}
}