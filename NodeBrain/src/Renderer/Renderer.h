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
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"

namespace NodeBrain
{
	struct VertexData
	{
		glm::vec3 Position;
		glm::vec2 UV;
		glm::vec3 Normal;

		VertexData(const glm::vec3& position, const glm::vec2& uv, const glm::vec3& normal)
			: Position(position), UV(uv), Normal(normal) {}
	};

	struct PushConstantData
	{
		glm::mat4 ViewProjectionMatrix;
		glm::mat4 ModelMatrix;;
	};



	struct RendererData
	{
		const uint32_t MaxQuads = 1000000;
		const uint32_t MaxQuadVertices = MaxQuads * 4;
		const uint32_t MaxQuadIndices = MaxQuads * 6;

		const uint32_t MaxCubes = 1000000;
		const uint32_t MaxCubeVertices = MaxCubes * 36;

		const uint32_t MaxMesh = 1000000;

		// --- Built-in Shaders ---
		std::shared_ptr<Shader> UnlitColorVertexShader;
		std::shared_ptr<Shader> UnlitColorFragmentShader;
		std::shared_ptr<GraphicsPipeline> UnlitColorPipeline;

		std::shared_ptr<Shader> UnlitTextureVertexShader;
		std::shared_ptr<Shader> UnlitTextureFragmentShader;
		std::shared_ptr<GraphicsPipeline> UnlitTexturePipeline;


		std::shared_ptr<UniformBuffer> PerObjectUBO;
		PushConstantData PushConstantBuffer;


		// --- Quad ---
		std::shared_ptr<VertexBuffer> QuadVertexBuffer;
		std::shared_ptr<IndexBuffer> QuadIndexBuffer;
		std::unordered_map<std::shared_ptr<Material>, std::vector<VertexData>> QuadVertexData; // TODO: Slow to use vector
		glm::vec3 QuadVertexPositions[4];
		glm::vec2 QuadTexCoords[4];


		// --- Cube ---
		std::shared_ptr<VertexBuffer> CubeVertexBuffer;
		std::unordered_map<std::shared_ptr<Material>, std::vector<VertexData>> CubeVertexData;
		glm::vec3 CubeVertexPositions[36];
		glm::vec3 CubeNormals[36];
		glm::vec2 CubeTexCoords[36];


		std::shared_ptr<Texture2D> BlankTexture;
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

		void SubmitQuad(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tint);
		void SubmitQuad(const glm::mat4& transform, const glm::vec4& color);
		void SubmitQuad(const glm::mat4& transform, const std::shared_ptr<Material>& material);

		void SubmitCube(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tint);
		void SubmitCube(const glm::mat4& transform, const std::shared_ptr<Material>& material);

		void DrawMesh(const glm::mat4& transform, const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material);


		[[nodiscard]] RenderContext& GetContext() const { return m_RendererAPI.GetContext(); };
		[[nodiscard]] RendererAPI& GetAPI() const { return m_RendererAPI; }

		[[nodiscard]] std::shared_ptr<GraphicsPipeline> GetPipelineByName(const std::string& name) const;

	private:
		RendererAPI& m_RendererAPI;
		RenderContext& m_Context;

		RendererData m_Data;
	};
}
