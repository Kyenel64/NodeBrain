#include "NBpch.h"
#include "Renderer.h"

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

	void Renderer::BeginScene(std::shared_ptr<Image> targetImage)
	{
		s_Data->TestPipeline->SetTargetImage(targetImage);

		s_RendererAPI->ClearColor({ 0.3f, 0.3f, 0.8f, 1.0f }, targetImage);
	}

	void Renderer::EndScene()
	{
		s_Data->TestPushConstantData.ViewMatrix = glm::mat4(1.0f);
		s_Data->TestPushConstantData.Address = s_Data->TestVertexBuffer->GetAddress();
		s_Data->TestPipeline->SetPushConstantData(&s_Data->TestPushConstantData, sizeof(PushConstantData), 0);
		float y = sin(glfwGetTime()) * 1.0f;
		s_Data->TestVertexData[0].Position = { y, -y, 0.0f };
		s_Data->TestVertexBuffer->SetData(s_Data->TestVertexData, sizeof(QuadVertex) * 4);


		s_RendererAPI->BeginRenderPass(s_Data->TestPipeline);
		s_RendererAPI->DrawIndexed(s_Data->TestIndexBuffer, 6, 0);
		s_RendererAPI->EndRenderPass(s_Data->TestPipeline);
	}


	// --- Backend ---
	void Renderer::WaitForGPU()
	{
		s_RendererAPI->WaitForGPU();
	}

	void Renderer::ClearColor(const glm::vec4& color, std::shared_ptr<Image> image)
	{
		s_RendererAPI->ClearColor(color, image);
	}

	void Renderer::BeginRenderPass(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		s_RendererAPI->BeginRenderPass(pipeline);
	}

	void Renderer::EndRenderPass(std::shared_ptr<GraphicsPipeline> pipeline)
	{
		s_RendererAPI->EndRenderPass(pipeline);
	}

	void Renderer::Draw(uint32_t vertexCount, uint32_t vertexIndex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		s_RendererAPI->Draw(vertexCount, vertexIndex, instanceCount, instanceIndex);
	}

	void Renderer::DrawIndexed(std::shared_ptr<IndexBuffer> indexBuffer, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount, uint32_t instanceIndex)
	{
		s_RendererAPI->DrawIndexed(indexBuffer, indexCount, firstIndex, instanceCount, instanceIndex);
	}

	void Renderer::BeginComputePass(std::shared_ptr<ComputePipeline> pipeline)
	{
		s_RendererAPI->BeginComputePass(pipeline);
	}

	void Renderer::EndComputePass(std::shared_ptr<ComputePipeline> pipeline)
	{
		s_RendererAPI->EndComputePass(pipeline);
	}

	void Renderer::DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ)
	{
		s_RendererAPI->DispatchCompute(groupX, groupY, groupZ);
	}


	// Temp
	void Renderer::TempUpdateImage(std::shared_ptr<Shader> shader, std::shared_ptr<Image> image)
	{
		s_RendererAPI->TempUpdateImage(shader, image);
	}

	void Renderer::ProcessGradientCompute()
	{
		s_Data->ColorGradientBuffer.TopColor = { 1, 0, 0, 1 };
		s_Data->ColorGradientBuffer.BottomColor = { 0, 0, 1, 1 };
		s_Data->ColorGradientPipeline->SetPushConstantData(&s_Data->ColorGradientBuffer, sizeof(ColorGradientData), 0);

		s_RendererAPI->BeginComputePass(s_Data->ColorGradientPipeline);
		uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
		uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
		s_RendererAPI->DispatchCompute(groupX, groupY, 1);
		s_RendererAPI->EndComputePass(s_Data->ColorGradientPipeline);
	}
	
	void Renderer::ProcessFlatColorCompute()
	{
		s_Data->Color = { 0, 1, 0, 1 };
		s_Data->FlatColorPipeline->SetPushConstantData(&s_Data->Color, sizeof(glm::vec4), 64);

		s_RendererAPI->BeginComputePass(s_Data->FlatColorPipeline);
		uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
		uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
		s_RendererAPI->DispatchCompute(groupX, groupY, 1);
		s_RendererAPI->EndComputePass(s_Data->ColorGradientPipeline);
	}
}