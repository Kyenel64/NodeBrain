#include "NBpch.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

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
		glm::mat4 ViewProjectionMatrix;
		uint64_t Address;
	};

	struct RendererData
	{
		const uint32_t MaxQuads = 20000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		PushConstantData PushConstantBuffer;

		// --- Quad ---
		std::shared_ptr<Shader> QuadVertexShader;
		std::shared_ptr<Shader> QuadFragmentShader;
		std::shared_ptr<GraphicsPipeline> QuadPipeline;

		uint32_t QuadIndexCount = 0;
		std::shared_ptr<VertexBuffer> QuadVertexBuffer;
		std::shared_ptr<IndexBuffer> QuadIndexBuffer;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		glm::vec3 QuadVertexPositions[4];


		// --- Color Gradient ---
		std::shared_ptr<Shader> ColorGradientShader;
		std::shared_ptr<ComputePipeline> ColorGradientPipeline;
		ColorGradientData ColorGradientBuffer;

	};

	static RendererData* s_Data = nullptr;

	GAPI Renderer::GetGAPI() { return s_GAPI; }

	void Renderer::Init()
	{
		NB_PROFILE_FN();

		s_Data = new RendererData();
		s_RendererAPI = RendererAPI::Create();


		// --- Quads ---
		s_Data->QuadVertexShader = Shader::Create("Assets/Shaders/Compiled/triangle.vert.spv", ShaderType::Vertex);
		s_Data->QuadVertexShader->SetPushConstantLayout(sizeof(PushConstantData), 0);
		s_Data->QuadFragmentShader = Shader::Create("Assets/Shaders/Compiled/triangle.frag.spv", ShaderType::Fragment);
		PipelineConfiguration pipelineConfig = {};
		pipelineConfig.VertexShader = s_Data->QuadVertexShader;
		pipelineConfig.FragmentShader = s_Data->QuadFragmentShader;
		s_Data->QuadPipeline = GraphicsPipeline::Create(pipelineConfig);

		s_Data->QuadVertexBuffer = VertexBuffer::Create(sizeof(QuadVertex) * s_Data->MaxVertices);

		uint32_t* quadIndices = new uint32_t[s_Data->MaxIndices];
		uint32_t offset = 0;
		for (size_t i = 0; i < s_Data->MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		s_Data->QuadIndexBuffer = IndexBuffer::Create(quadIndices, sizeof(uint32_t) * s_Data->MaxIndices);
		delete[] quadIndices;

		s_Data->QuadVertexBufferBase = new QuadVertex[s_Data->MaxVertices];

		s_Data->QuadVertexPositions[0] = { -0.5f,  0.5f,  0.0f };
		s_Data->QuadVertexPositions[1] = { -0.5f, -0.5f,  0.0f };
		s_Data->QuadVertexPositions[2] = {  0.5f, -0.5f,  0.0f };
		s_Data->QuadVertexPositions[3] = {  0.5f,  0.5f,  0.0f };



		// --- Color Gradient ---
		s_Data->ColorGradientShader = Shader::Create("Assets/Shaders/Compiled/gradientColor.comp.spv", ShaderType::Compute);
		// TODO: Setting layouts will most likely be done automatically by parsing shader file.
		s_Data->ColorGradientShader->SetLayout({ { BindingType::StorageImage, 1} });
		s_Data->ColorGradientShader->SetPushConstantLayout(sizeof(ColorGradientData), 0);
		s_Data->ColorGradientPipeline = ComputePipeline::Create(s_Data->ColorGradientShader);
		s_RendererAPI->TempUpdateImage(s_Data->ColorGradientShader);


		NB_INFO("Initialized renderer");
	}

	void Renderer::Shutdown()
	{
		NB_PROFILE_FN();

		delete s_Data->QuadVertexBufferBase;
		s_Data->QuadVertexBufferBase = nullptr;

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
		s_Data->QuadPipeline->SetTargetImage(targetImage);

		s_RendererAPI->ClearColor({ 0.3f, 0.3f, 0.8f, 1.0f }, targetImage);

		s_Data->PushConstantBuffer.ViewProjectionMatrix = glm::mat4(1.0f);
		s_Data->PushConstantBuffer.Address = s_Data->QuadVertexBuffer->GetAddress();
		s_Data->QuadPipeline->SetPushConstantData(&s_Data->PushConstantBuffer, sizeof(PushConstantData), 0);

		s_Data->QuadIndexCount = 0;
		s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
	}

	void Renderer::EndScene()
	{
		RenderSubmitted();
	}

	void Renderer::SubmitQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		// New batch
		if (s_Data->QuadIndexCount >= s_Data->MaxIndices)
		{
			RenderSubmitted();
			s_Data->QuadIndexCount = 0;
			s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
		}
			

		for (size_t i = 0; i < 4; i++)
		{
			s_Data->QuadVertexBufferPtr->Position = transform * glm::vec4(s_Data->QuadVertexPositions[i], 1.0f);
			s_Data->QuadVertexBufferPtr->Color = color;
			s_Data->QuadVertexBufferPtr->Normal = glm::vec3(1.0f);
			s_Data->QuadVertexBufferPtr->UVX = 0; // temp
			s_Data->QuadVertexBufferPtr->UVY = 0;

			s_Data->QuadVertexBufferPtr++;
		}

		s_Data->QuadIndexCount += 6;
	}

	void Renderer::RenderSubmitted()
	{
		if (s_Data->QuadIndexCount)
		{
			uint32_t size = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
			s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, size);

			s_RendererAPI->BeginRenderPass(s_Data->QuadPipeline);
			s_RendererAPI->DrawIndexed(s_Data->QuadIndexBuffer, s_Data->QuadIndexCount, 0);
			s_RendererAPI->EndRenderPass(s_Data->QuadPipeline);
		}
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
}