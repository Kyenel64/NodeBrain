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
#include "Renderer/Image.h"
#include "Renderer/EditorCamera.h"
#include "Renderer/Framebuffer.h"

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

		std::shared_ptr<Image> WhiteTexture;


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

		[[nodiscard]] RenderContext& GetContext() const { return m_RendererAPI.GetContext(); };
		[[nodiscard]] RendererAPI& GetAPI() const { return m_RendererAPI; }

	private:
		RendererAPI& m_RendererAPI;
		RenderContext& m_Context;

		RendererData m_Data;
	};
}