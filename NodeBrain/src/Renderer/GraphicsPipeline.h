#pragma once

#include "Renderer/Shader.h"

namespace NodeBrain
{
	enum class TopologyType { None = 0, PointList, LineList, LineStrip, TriangleList, TriangleStrip };
	enum class PolygonFillMode { None = 0, Point, Line, Fill };

	struct PipelineConfiguration
	{
		std::shared_ptr<Shader> VertexShader;
		std::shared_ptr<Shader> FragmentShader;

		std::shared_ptr<Image> TargetImage = nullptr;

		TopologyType Topology = TopologyType::TriangleList;
		PolygonFillMode FillMode = PolygonFillMode::Fill;
		float LineWidth = 1.0f;
		// TODO:
		// ColorBlendingMode
	};

	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline() = default;

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) = 0;
		virtual void SetTargetImage(std::shared_ptr<Image> targetImage) = 0;

		virtual const PipelineConfiguration& GetConfiguration() const = 0;
		
		static std::shared_ptr<GraphicsPipeline> Create(const PipelineConfiguration& configuration);
	};
}