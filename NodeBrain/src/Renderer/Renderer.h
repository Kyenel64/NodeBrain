#pragma once

#include <glm/glm.hpp>

#include "Renderer/RendererAPI.h"
#include "Renderer/RenderContext.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/ComputePipeline.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/DescriptorSet.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/Texture2D.h"

namespace NodeBrain
{
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

	struct TestUniformData
	{
		glm::vec4 Color;
	};

	struct RendererData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxVertices = MaxQuads * 4;
		const uint32_t MaxIndices = MaxQuads * 6;

		const uint32_t MaxTextures = 16; // TODO: depends on maxPerStageDescriptorSampledImages

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
		glm::vec2 QuadTextureCoords[4];

		std::shared_ptr<Texture2D> WhiteTexture;
		std::vector<std::shared_ptr<Texture2D>> Textures; // MaxTextures
		uint32_t TextureIndex = 1;


		TestUniformData TestUniformDataBuffer;
		std::shared_ptr<UniformBuffer> TestUniformBuffer;
		std::shared_ptr<DescriptorSet> GlobalDescriptorSet;
	};

	class Renderer
	{
	public:
		explicit Renderer(RendererAPI& rendererAPI);
		~Renderer();

		void BeginFrame();
		void EndFrame();

		void BeginScene(const std::shared_ptr<EditorCamera>& editorCamera, const std::shared_ptr<Framebuffer>& targetFramebuffer = nullptr);
		void EndScene();

		void RenderSubmitted();

		void SubmitQuad(const glm::mat4& transform, const glm::vec4& color);
		void SubmitQuad(const glm::mat4& transform, const glm::vec4& color, const std::shared_ptr<Texture2D>& texture);

		[[nodiscard]] RenderContext& GetContext() const { return m_RendererAPI.GetContext(); };
		[[nodiscard]] RendererAPI& GetAPI() const { return m_RendererAPI; }

	private:
		RendererAPI& m_RendererAPI;
		RenderContext& m_Context;

		RendererData m_Data;
	};
}
