#pragma once

#include <glm/glm.hpp>

#include "Renderer/GraphicsPipeline.h"
#include "Renderer/ComputePipeline.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Image.h"

namespace NodeBrain
{
	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void ClearColor(const glm::vec4& color, const std::shared_ptr<Image>& image = nullptr) = 0;

		virtual void BeginRenderPass(const std::shared_ptr<GraphicsPipeline>& pipeline) = 0;
		virtual void EndRenderPass(const std::shared_ptr<GraphicsPipeline>& pipeline) = 0;
		virtual void Draw(uint32_t vertexCount, uint32_t firstVertex = 0, uint32_t instanceCount = 1, uint32_t instanceIndex = 0) = 0;
		virtual void DrawIndexed(const std::shared_ptr<IndexBuffer>& indexBuffer, uint32_t indexCount, uint32_t firstIndex, uint32_t instanceCount = 1, uint32_t instanceIndex = 0) = 0;

		virtual void BeginComputePass(const std::shared_ptr<ComputePipeline>& pipeline) = 0;
		virtual void EndComputePass(const std::shared_ptr<ComputePipeline>& pipeline) = 0;
		virtual void DispatchCompute(uint32_t groupX, uint32_t groupY, uint32_t groupZ) = 0;

		virtual RenderContext* GetContext() const = 0;
	};
}