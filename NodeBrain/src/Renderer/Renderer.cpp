#include "NBpch.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace NodeBrain
{
	Renderer::Renderer(RendererAPI& rendererAPI)
		: m_RendererAPI(rendererAPI), m_Context(rendererAPI.GetContext())
	{		
		NB_PROFILE_FN();

		// --- Uniforms ---
		m_Data.TestUniformBuffer = UniformBuffer::Create(m_Context, nullptr, sizeof(TestUniformData));

		// --- Textures ---
		ImageConfiguration whiteTextureConfig = {};
		whiteTextureConfig.Width = 1;
		whiteTextureConfig.Height = 1;
		whiteTextureConfig.Format = ImageFormat::RGBA8;
		uint32_t whiteTextureData = 0xFFFFFFFF;
		m_Data.WhiteTexture = Image::Create(m_Context, whiteTextureConfig);
		m_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

		// --- Descriptor Sets ---
		m_Data.GlobalDescriptorSet = DescriptorSet::Create(m_Context, {
			{ BindingType::UniformBuffer, 0 },
			{ BindingType::ImageSampler, 1 }});
		m_Data.GlobalDescriptorSet->WriteBuffer(m_Data.TestUniformBuffer, 0);
		m_Data.GlobalDescriptorSet->WriteSampler(m_Data.WhiteTexture, 1);

		
		// --- Quads ---
		m_Data.QuadVertexShader = Shader::Create(m_Context, "Assets/Shaders/Compiled/quad.vert.spv", ShaderType::Vertex);
		m_Data.QuadFragmentShader = Shader::Create(m_Context, "Assets/Shaders/Compiled/quad.frag.spv", ShaderType::Fragment);
		GraphicsPipelineConfiguration pipelineConfig = {};
		pipelineConfig.VertexShader = m_Data.QuadVertexShader;
		pipelineConfig.FragmentShader = m_Data.QuadFragmentShader;
		pipelineConfig.AddDescriptorSet(m_Data.GlobalDescriptorSet, 0);
		m_Data.QuadPipeline = GraphicsPipeline::Create(m_Context, pipelineConfig);


		m_Data.QuadVertexBuffer = VertexBuffer::Create(m_Context, nullptr, sizeof(QuadVertex) * m_Data.MaxVertices);

		auto* quadIndices = new uint32_t[m_Data.MaxIndices];
		uint32_t offset = 0;
		for (size_t i = 0; i < m_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		m_Data.QuadIndexBuffer = IndexBuffer::Create(m_Context, quadIndices, sizeof(uint32_t) * m_Data.MaxIndices);
		delete[] quadIndices;

		m_Data.QuadVertexBufferBase = new QuadVertex[m_Data.MaxVertices];

		m_Data.QuadVertexPositions[0] = { -0.5f,  0.5f,  0.0f };
		m_Data.QuadVertexPositions[1] = { -0.5f, -0.5f,  0.0f };
		m_Data.QuadVertexPositions[2] = {  0.5f, -0.5f,  0.0f };
		m_Data.QuadVertexPositions[3] = {  0.5f,  0.5f,  0.0f };


		NB_INFO("Initialized renderer");
	}

	Renderer::~Renderer()
	{
		NB_PROFILE_FN();

		m_Context.WaitForGPU();

		delete m_Data.QuadVertexBufferBase;
		m_Data.QuadVertexBufferBase = nullptr;

		NB_INFO("Shutdown renderer");
	}

	void Renderer::BeginFrame()
	{
		NB_PROFILE_FN();

		m_RendererAPI.BeginFrame();

		// Bind per frame descriptors
		m_Data.QuadPipeline->BindDescriptorSet(m_Data.GlobalDescriptorSet);
	}

	void Renderer::EndFrame()
	{
		m_RendererAPI.EndFrame();
	}

	void Renderer::BeginScene(const std::shared_ptr<EditorCamera>& editorCamera, const std::shared_ptr<Image>& targetImage)
	{
		NB_PROFILE_FN();

		m_Data.QuadPipeline->SetTargetImage(targetImage);

		m_RendererAPI.ClearColor({ 0.3f, 0.3f, 0.8f, 1.0f }, targetImage);

		m_Data.PushConstantBuffer.ViewProjectionMatrix = editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix();

		m_Data.TestUniformDataBuffer.Color = { 0, 1, 0, 1.0f };
		m_Data.TestUniformBuffer->SetData(&m_Data.TestUniformDataBuffer, sizeof(TestUniformData));

		m_Data.PushConstantBuffer.Address = m_Data.QuadVertexBuffer->GetAddress();
		m_Data.QuadPipeline->SetPushConstantData(&m_Data.PushConstantBuffer, sizeof(PushConstantData), 0);


		m_Data.QuadIndexCount = 0;
		m_Data.QuadVertexBufferPtr = m_Data.QuadVertexBufferBase;
	}

	void Renderer::EndScene()
	{
		NB_PROFILE_FN();

		RenderSubmitted();
	}

	void Renderer::SubmitQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		NB_PROFILE_FN();

		// New batch
		if (m_Data.QuadIndexCount >= m_Data.MaxIndices)
		{
			RenderSubmitted();
			m_Data.QuadIndexCount = 0;
			m_Data.QuadVertexBufferPtr = m_Data.QuadVertexBufferBase;
		}
			

		for (size_t i = 0; i < 4; i++)
		{
			m_Data.QuadVertexBufferPtr->Position = transform * glm::vec4(m_Data.QuadVertexPositions[i], 1.0f);
			m_Data.QuadVertexBufferPtr->Color = color;
			m_Data.QuadVertexBufferPtr->Normal = glm::vec3(1.0f);
			m_Data.QuadVertexBufferPtr->UVX = 0; // temp
			m_Data.QuadVertexBufferPtr->UVY = 0;

			m_Data.QuadVertexBufferPtr++;
		}

		m_Data.QuadIndexCount += 6;
	}

	void Renderer::RenderSubmitted()
	{
		NB_PROFILE_FN();

		if (m_Data.QuadIndexCount)
		{
			uint32_t size = (uint32_t)((uint8_t*)m_Data.QuadVertexBufferPtr - (uint8_t*)m_Data.QuadVertexBufferBase);
			m_Data.QuadVertexBuffer->SetData(m_Data.QuadVertexBufferBase, size);

			m_RendererAPI.BeginRenderPass(m_Data.QuadPipeline);
			m_RendererAPI.DrawIndexed(m_Data.QuadIndexBuffer, m_Data.QuadIndexCount, 0);
			m_RendererAPI.EndRenderPass(m_Data.QuadPipeline);
		}
	}
}