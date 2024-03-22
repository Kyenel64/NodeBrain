#pragma once

#include "Renderer/Shader.h"
#include "Renderer/Framebuffer.h"

namespace NodeBrain
{
	enum class TopologyType { None = 0, PointList, LineList, LineStrip, TriangleList, TriangleStrip };
	enum class PolygonFillMode { None = 0, Point, Line, Fill };

	struct PipelineConfiguration
	{
		std::shared_ptr<Shader> VertexShader;
		std::shared_ptr<Shader> FragmentShader;

		// Uses swapchain renderpass if render pass not provided
		std::shared_ptr<Framebuffer> Framebuffer = nullptr;

		TopologyType Topology = TopologyType::TriangleList;
		PolygonFillMode PolygonFillMode = PolygonFillMode::Fill;
		float LineWidth = 1.0f;
		// TODO:
		// ColorBlendingMode
		// Uniforms & Push constants
	};

	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline() = default;

		virtual const PipelineConfiguration& GetConfiguration() const = 0;
		static std::shared_ptr<GraphicsPipeline> Create(const PipelineConfiguration& configuration);
	};
}