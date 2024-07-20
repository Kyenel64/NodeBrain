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
	struct VertexData
	{
		glm::vec3 Position;
		float UVX;
		glm::vec3 Normal;
		float UVY;
		glm::vec4 Color;
		glm::ivec4 TexIndex; // TODO: int + vec3 doesn't work for some reason.
	};

	struct PushConstantData
	{
		glm::mat4 ViewProjectionMatrix;
		uint64_t Address;
	};


	struct RendererData
	{
		const uint32_t MaxQuads = 10000;
		const uint32_t MaxQuadVertices = MaxQuads * 4;
		const uint32_t MaxQuadIndices = MaxQuads * 6;

		const uint32_t MaxCubes = 5000;
		const uint32_t MaxCubeVertices = MaxCubes * 36;

		const uint32_t MaxTextures = 16; // TODO: depends on maxPerStageDescriptorSampledImages

		// --- Built-in Descriptors ---
		std::shared_ptr<DescriptorSet> TextureDescriptorSet;

		// --- Built-in Shaders ---
		std::shared_ptr<Shader> UnlitVertexShader;
		std::shared_ptr<Shader> UnlitFragmentShader;
		std::shared_ptr<GraphicsPipeline> UnlitPipeline;





		PushConstantData PushConstantBuffer;
		PushConstantData PushConstantBuffer2;

		// --- Quad ---
		uint32_t QuadIndexCount = 0;
		std::shared_ptr<VertexBuffer> QuadVertexBuffer;
		std::shared_ptr<IndexBuffer> QuadIndexBuffer;
		VertexData* QuadVertexBufferBase = nullptr;
		VertexData* QuadVertexBufferPtr = nullptr;

		glm::vec3 QuadVertexPositions[4];
		glm::vec2 QuadTextureCoords[4];


		// --- Cube ---
		uint32_t CubeVertexCount = 0;
		std::shared_ptr<VertexBuffer> CubeVertexBuffer;
		VertexData* CubeVertexBufferBase = nullptr;
		VertexData* CubeVertexBufferPtr = nullptr;

		glm::vec3 CubeVertexPositions[36];
		glm::vec3 CubeNormals[6];
		glm::vec2 CubeTexCoords[6];


		std::shared_ptr<Texture2D> BlankTexture;
		std::vector<std::shared_ptr<Texture2D>> Textures; // MaxTextures
		uint32_t TextureIndex = 1;

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
		void SubmitQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tint);

		void SubmitCube(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tint);

		[[nodiscard]] RenderContext& GetContext() const { return m_RendererAPI.GetContext(); };
		[[nodiscard]] RendererAPI& GetAPI() const { return m_RendererAPI; }

	private:
		RendererAPI& m_RendererAPI;
		RenderContext& m_Context;

		RendererData m_Data;
	};
}
