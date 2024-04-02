#pragma once

#include <glm/glm.hpp>

#include "Renderer/GraphicsPipeline.h"
#include "Renderer/Framebuffer.h"

namespace NodeBrain
{
	class RendererAPI
	{
	public:
		virtual ~RendererAPI() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void BeginRenderPass(std::shared_ptr<RenderPass> renderPass = nullptr) = 0;
		virtual void EndRenderPass() = 0;

		virtual void BeginDynamicPass() = 0;
		virtual void EndDynamicPass() = 0;
		virtual void ClearColor(const glm::vec4& color) = 0;

		virtual void DrawTestTriangle(std::shared_ptr<GraphicsPipeline> pipeline) = 0;

		virtual void WaitForGPU() = 0;

		static std::unique_ptr<RendererAPI> Create();
	};
}