#include "NBpch.h"
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer/Mesh.h"
#include "Scene/Component.h"

namespace NodeBrain
{
	Renderer::Renderer(RendererAPI& rendererAPI)
		: m_RendererAPI(rendererAPI), m_Context(rendererAPI.GetContext())
	{		
		NB_PROFILE_FN();

		// --- Globals ---
		m_Data.PerObjectUBO = UniformBuffer::Create(m_Context, nullptr, m_Data.MaxCubes * sizeof(glm::mat4));

		// --- Shaders ---
		// Unlit Color
		m_Data.UnlitColorVertexShader = Shader::Create(m_Context, "Assets/Shaders/Compiled/UnlitColor.vert.spv", ShaderType::Vertex);
		m_Data.UnlitColorFragmentShader = Shader::Create(m_Context, "Assets/Shaders/Compiled/UnlitColor.frag.spv", ShaderType::Fragment);
		GraphicsPipelineConfiguration unlitColorPipelineConfig = {};
		unlitColorPipelineConfig.VertexShader = m_Data.UnlitColorVertexShader;
		unlitColorPipelineConfig.FragmentShader = m_Data.UnlitColorFragmentShader;
		m_Data.UnlitColorPipeline = GraphicsPipeline::Create(m_Context, unlitColorPipelineConfig);

		// Unlit Texture
		m_Data.UnlitTextureVertexShader = Shader::Create(m_Context, "Assets/Shaders/Compiled/UnlitTexture.vert.spv", ShaderType::Vertex);
		m_Data.UnlitTextureFragmentShader = Shader::Create(m_Context, "Assets/Shaders/Compiled/UnlitTexture.frag.spv", ShaderType::Fragment);
		GraphicsPipelineConfiguration unlitTexturePipelineConfig = {};
		unlitTexturePipelineConfig.VertexShader = m_Data.UnlitTextureVertexShader;
		unlitTexturePipelineConfig.FragmentShader = m_Data.UnlitTextureFragmentShader;
		m_Data.UnlitTexturePipeline = GraphicsPipeline::Create(m_Context, unlitTexturePipelineConfig);


		// --- Quad ---
		m_Data.QuadVertexBuffer = VertexBuffer::Create(m_Context, nullptr, sizeof(VertexData) * m_Data.MaxQuadVertices);
		auto* quadIndices = new uint32_t[m_Data.MaxQuadIndices];
		uint32_t offset = 0;
		for (size_t i = 0; i < m_Data.MaxQuadIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;
			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}
		m_Data.QuadIndexBuffer = IndexBuffer::Create(m_Context, quadIndices, sizeof(uint32_t) * m_Data.MaxQuadIndices);
		delete[] quadIndices;

		m_Data.QuadVertexBufferBase = new VertexData[m_Data.MaxQuadVertices];

#pragma region Quad Vertex Definitions
		m_Data.QuadVertexPositions[0] = { -0.5f, -0.5f,  0.0f };
		m_Data.QuadVertexPositions[1] = {  0.5f, -0.5f,  0.0f };
		m_Data.QuadVertexPositions[2] = {  0.5f,  0.5f,  0.0f };
		m_Data.QuadVertexPositions[3] = { -0.5f,  0.5f,  0.0f };

		m_Data.QuadTextureCoords[0] = { 0.0f, 0.0f };
		m_Data.QuadTextureCoords[1] = { 1.0f, 0.0f };
		m_Data.QuadTextureCoords[2] = { 1.0f, 1.0f };
		m_Data.QuadTextureCoords[3] = { 0.0f, 1.0f };

#pragma endregion Quad Vertex Definitions


		// --- Cube ---
		m_Data.CubeVertexBuffer = VertexBuffer::Create(m_Context, nullptr, sizeof(VertexData) * m_Data.MaxCubeVertices); // TODO: Max cube
		m_Data.CubeVertexBufferBase = new VertexData[m_Data.MaxCubeVertices];

#pragma region Cube Vertex Definitions
		glm::vec4 vertexPos[8] = {};
		vertexPos[0] = { -0.5f, -0.5f, -0.5f, 1.0f }; // Front Bottom Left
		vertexPos[1] = {  0.5f, -0.5f, -0.5f, 1.0f }; // Front Bottom Right
		vertexPos[2] = {  0.5f,  0.5f, -0.5f, 1.0f }; // Front Top Right
		vertexPos[3] = { -0.5f,  0.5f, -0.5f, 1.0f }; // Front Top Left
		vertexPos[4] = { -0.5f, -0.5f,  0.5f, 1.0f }; // Bottom Left Back
		vertexPos[5] = {  0.5f, -0.5f,  0.5f, 1.0f }; // Top Right Back
		vertexPos[6] = {  0.5f,  0.5f,  0.5f, 1.0f }; // Bottom Right Back
		vertexPos[7] = { -0.5f,  0.5f,  0.5f, 1.0f }; // Top Left Back

		// Front
		m_Data.CubeVertexPositions[0] = vertexPos[0];
		m_Data.CubeVertexPositions[1] = vertexPos[1];
		m_Data.CubeVertexPositions[2] = vertexPos[2];
		m_Data.CubeVertexPositions[3] = vertexPos[2];
		m_Data.CubeVertexPositions[4] = vertexPos[3];
		m_Data.CubeVertexPositions[5] = vertexPos[0];
		m_Data.CubeTexCoords[0] = { 0, 0 };
		m_Data.CubeTexCoords[1] = { 1, 0 };
		m_Data.CubeTexCoords[2] = { 1, 1 };
		m_Data.CubeTexCoords[3] = { 1, 1 };
		m_Data.CubeTexCoords[4] = { 0, 1 };
		m_Data.CubeTexCoords[5] = { 0, 0 };

		// Back
		m_Data.CubeVertexPositions[6] = vertexPos[5];
		m_Data.CubeVertexPositions[7] = vertexPos[4];
		m_Data.CubeVertexPositions[8] = vertexPos[7];
		m_Data.CubeVertexPositions[9] = vertexPos[7];
		m_Data.CubeVertexPositions[10] = vertexPos[6];
		m_Data.CubeVertexPositions[11] = vertexPos[5];
		m_Data.CubeTexCoords[6] = { 0, 0 };
		m_Data.CubeTexCoords[7] = { 1, 0 };
		m_Data.CubeTexCoords[8] = { 1, 1 };
		m_Data.CubeTexCoords[9] = { 1, 1 };
		m_Data.CubeTexCoords[10] = { 0, 1 };
		m_Data.CubeTexCoords[11] = { 0, 0 };

		// Left
		m_Data.CubeVertexPositions[12] = vertexPos[4];
		m_Data.CubeVertexPositions[13] = vertexPos[0];
		m_Data.CubeVertexPositions[14] = vertexPos[3];
		m_Data.CubeVertexPositions[15] = vertexPos[3];
		m_Data.CubeVertexPositions[16] = vertexPos[7];
		m_Data.CubeVertexPositions[17] = vertexPos[4];
		m_Data.CubeTexCoords[12] = { 0, 0 };
		m_Data.CubeTexCoords[13] = { 1, 0 };
		m_Data.CubeTexCoords[14] = { 1, 1 };
		m_Data.CubeTexCoords[15] = { 1, 1 };
		m_Data.CubeTexCoords[16] = { 0, 1 };
		m_Data.CubeTexCoords[17] = { 0, 0 };

		// Right
		m_Data.CubeVertexPositions[18] = vertexPos[1];
		m_Data.CubeVertexPositions[19] = vertexPos[5];
		m_Data.CubeVertexPositions[20] = vertexPos[6];
		m_Data.CubeVertexPositions[21] = vertexPos[6];
		m_Data.CubeVertexPositions[22] = vertexPos[2];
		m_Data.CubeVertexPositions[23] = vertexPos[1];
		m_Data.CubeTexCoords[18] = { 0, 0 };
		m_Data.CubeTexCoords[19] = { 1, 0 };
		m_Data.CubeTexCoords[20] = { 1, 1 };
		m_Data.CubeTexCoords[21] = { 1, 1 };
		m_Data.CubeTexCoords[22] = { 0, 1 };
		m_Data.CubeTexCoords[23] = { 0, 0 };

		// Top
		m_Data.CubeVertexPositions[24] = vertexPos[3];
		m_Data.CubeVertexPositions[25] = vertexPos[2];
		m_Data.CubeVertexPositions[26] = vertexPos[6];
		m_Data.CubeVertexPositions[27] = vertexPos[6];
		m_Data.CubeVertexPositions[28] = vertexPos[7];
		m_Data.CubeVertexPositions[29] = vertexPos[3];
		m_Data.CubeTexCoords[24] = { 0, 0 };
		m_Data.CubeTexCoords[25] = { 1, 0 };
		m_Data.CubeTexCoords[26] = { 1, 1 };
		m_Data.CubeTexCoords[27] = { 1, 1 };
		m_Data.CubeTexCoords[28] = { 0, 1 };
		m_Data.CubeTexCoords[29] = { 0, 0 };

		// Bottom
		m_Data.CubeVertexPositions[30] = vertexPos[4];
		m_Data.CubeVertexPositions[31] = vertexPos[5];
		m_Data.CubeVertexPositions[32] = vertexPos[1];
		m_Data.CubeVertexPositions[33] = vertexPos[1];
		m_Data.CubeVertexPositions[34] = vertexPos[0];
		m_Data.CubeVertexPositions[35] = vertexPos[4];
		m_Data.CubeTexCoords[30] = { 0, 0 };
		m_Data.CubeTexCoords[31] = { 1, 0 };
		m_Data.CubeTexCoords[32] = { 1, 1 };
		m_Data.CubeTexCoords[33] = { 1, 1 };
		m_Data.CubeTexCoords[34] = { 0, 1 };
		m_Data.CubeTexCoords[35] = { 0, 0 };

#pragma endregion Cube Vertex Definitions


		// --- Textures ---
		Texture2DConfiguration blankTextureConfig = {};
		blankTextureConfig.Width = 1;
		blankTextureConfig.Height = 1;
		blankTextureConfig.Format = ImageFormat::RGBA8;
		uint32_t whiteTextureData = 0xFFFFFFFF;
		m_Data.BlankTexture = Texture2D::Create(m_Context, blankTextureConfig);
		m_Data.BlankTexture->SetData(&whiteTextureData, sizeof(uint32_t));
		// Set all textures to blank. Null descriptor may be used but might not be compatible with mac.
		m_Data.Textures.resize(m_Data.MaxTextures);
		for (size_t i = 0; i < m_Data.MaxTextures; i++)
			m_Data.Textures[i] = m_Data.BlankTexture;

		NB_INFO("Initialized renderer");
	}

	Renderer::~Renderer()
	{
		NB_PROFILE_FN();

		m_Context.WaitForGPU();

		delete m_Data.QuadVertexBufferBase;
		m_Data.QuadVertexBufferBase = nullptr;

		delete m_Data.CubeVertexBufferBase;
		m_Data.CubeVertexBufferBase = nullptr;

		NB_INFO("Shutdown renderer");
	}

	void Renderer::BeginFrame()
	{
		NB_PROFILE_FN();

		m_RendererAPI.BeginFrame();
	}

	void Renderer::EndFrame()
	{
		m_RendererAPI.EndFrame();
	}

	void Renderer::BeginScene(const std::shared_ptr<EditorCamera>& editorCamera, const std::shared_ptr<Framebuffer>& targetFramebuffer)
	{
		NB_PROFILE_FN();

		m_Data.UnlitColorPipeline->SetTargetFramebuffer(targetFramebuffer);

		m_RendererAPI.ClearColor({ 0.3f, 0.3f, 0.8f, 1.0f }, targetFramebuffer);

		m_Data.PushConstantBuffer.ViewProjectionMatrix = editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix();

		m_Data.QuadIndexCount = 0;
		m_Data.QuadVertexBufferPtr = m_Data.QuadVertexBufferBase;

		m_Data.CubeVertexCount = 0;
		m_Data.CubeVertexBufferPtr = m_Data.CubeVertexBufferBase;

		m_Data.TextureIndex = 1;

		m_Data.ObjectCount = 0;

		m_Data.MaterialBatches.clear();
	}

	void Renderer::EndScene()
	{
		NB_PROFILE_FN();

		RenderSubmitted();
	}

	void Renderer::SubmitQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tint)
	{
		NB_PROFILE_FN();

		// New batch
		if (m_Data.QuadIndexCount >= m_Data.MaxQuadIndices)
		{
			RenderSubmitted();
			m_Data.QuadIndexCount = 0;
			m_Data.QuadVertexBufferPtr = m_Data.QuadVertexBufferBase;

			m_Data.TextureIndex = 1;
		}

		uint32_t texIndex = 0;
		if (texture)
		{
			m_Data.Textures[m_Data.TextureIndex] = texture;
			texIndex = m_Data.TextureIndex;
			m_Data.TextureIndex++;
		}


		for (size_t i = 0; i < 4; i++)
		{
			m_Data.QuadVertexBufferPtr->Position = transform * glm::vec4(m_Data.QuadVertexPositions[i], 1.0f);
			m_Data.QuadVertexBufferPtr->Normal = { 1.0f, 1.0f, 1.0f };
			m_Data.QuadVertexBufferPtr->UVX = m_Data.QuadTextureCoords[i].x;
			m_Data.QuadVertexBufferPtr->UVY = m_Data.QuadTextureCoords[i].y;

			m_Data.QuadVertexBufferPtr++;
		}

		m_Data.QuadIndexCount += 6;
	}

	void Renderer::SubmitQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		NB_PROFILE_FN();

		SubmitQuad(transform, nullptr, color);
	}

	void Renderer::SubmitQuad(const glm::mat4& transform, const std::shared_ptr<Material>& material)
	{
		NB_PROFILE_FN();

		for (size_t i = 0; i < 4; i++)
		{
			m_Data.MaterialBatches[material].push_back({ transform * glm::vec4(m_Data.QuadVertexPositions[i], 1.0f),
				m_Data.QuadTextureCoords[i].x, glm::vec3(1.0f, 1.0f, 1.0f), m_Data.QuadTextureCoords[i].y });
		}

		m_Data.QuadIndexCount += 6;
	}

	void Renderer::SubmitCube(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tint)
	{
		NB_PROFILE_FN();

		// New batch TODO

		uint32_t texIndex = 0;
		if (texture)
		{
			m_Data.Textures[m_Data.TextureIndex] = texture;
			texIndex = m_Data.TextureIndex;
			m_Data.TextureIndex++;
		}


		for (size_t i = 0; i < 6; i++)
		{
			for (size_t j = 0; j < 6; j++)
			{
				const int index = (i * 6) + j;

				m_Data.CubeVertexBufferPtr->Position = transform * glm::vec4(m_Data.CubeVertexPositions[index], 1.0f);
				m_Data.CubeVertexBufferPtr->Normal = m_Data.CubeNormals[index];
				m_Data.CubeVertexBufferPtr->UVX = m_Data.CubeTexCoords[index].x;
				m_Data.CubeVertexBufferPtr->UVY = m_Data.CubeTexCoords[index].y;
				m_Data.CubeVertexBufferPtr++;
			}
		}

		m_Data.CubeVertexCount += 36;
	}

	void Renderer::SubmitCube(const glm::mat4& transform, const std::shared_ptr<Material>& material)
	{
		NB_PROFILE_FN();

		SubmitCube(transform, material);
	}

	void Renderer::RenderSubmitted()
	{
		NB_PROFILE_FN();

		for (auto& [material, vertices] : m_Data.MaterialBatches)
		{
			std::shared_ptr<GraphicsPipeline> pipeline = material->GetPipeline();
			const std::shared_ptr<DescriptorSet>& descriptorSet = material->GetDescriptorSet();

			m_Data.QuadVertexBuffer->SetData(vertices.data(), sizeof(VertexData) * vertices.size());

			m_Data.PerObjectUBO->SetData(glm::value_ptr(glm::mat4(1.0f)), sizeof(glm::mat4), m_Data.ObjectCount * sizeof(glm::mat4));
			descriptorSet->WriteBuffer(m_Data.PerObjectUBO, 0, sizeof(glm::mat4));

			pipeline->BindDescriptorSet(descriptorSet, 0);

			m_Data.PushConstantBuffer.Address = m_Data.QuadVertexBuffer->GetAddress();
			pipeline->SetPushConstantData(&m_Data.PushConstantBuffer, sizeof(PushConstantData), 0);

			m_RendererAPI.BeginRenderPass(pipeline);
			m_RendererAPI.DrawIndexed(m_Data.QuadIndexBuffer, m_Data.QuadIndexCount, 0);
			m_RendererAPI.EndRenderPass(pipeline);
		}
	}

	void Renderer::DrawMesh(const glm::mat4& transform, const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material)
	{
		const std::shared_ptr<GraphicsPipeline>& pipeline = material->GetPipeline();
		const std::shared_ptr<DescriptorSet>& descriptorSet = material->GetDescriptorSet();

		m_Data.PerObjectUBO->SetData(glm::value_ptr(transform), sizeof(glm::mat4), m_Data.ObjectCount * sizeof(glm::mat4));
		descriptorSet->WriteBuffer(m_Data.PerObjectUBO, 0, sizeof(glm::mat4), m_Data.ObjectCount * sizeof(glm::mat4));

		pipeline->BindDescriptorSet(descriptorSet, 0);

		m_Data.PushConstantBuffer.Address = mesh->GetVertexBuffer()->GetAddress();
		m_Data.UnlitColorPipeline->SetPushConstantData(&m_Data.PushConstantBuffer, sizeof(PushConstantData), 0);

		m_RendererAPI.BeginRenderPass(pipeline);
		m_RendererAPI.DrawIndexed(mesh->GetIndexBuffer(), mesh->GetIndexBuffer()->GetSize(), 0);
		m_RendererAPI.EndRenderPass(pipeline);

		m_Data.ObjectCount++;
	}

	std::shared_ptr<GraphicsPipeline> Renderer::GetPipelineByName(const std::string& name) const
	{
		if (name == "UnlitColor")
			return m_Data.UnlitColorPipeline;
		else if (name == "UnlitTexture")
			return m_Data.UnlitTexturePipeline;

		NB_ASSERT(false, "Unable to find pipeline from provided name.");

		return nullptr;
	}
}
