#pragma once

#include "Renderer/Shader.h"

namespace NodeBrain
{
	enum class TopologyType { None = 0, PointList, LineList, LineStrip, TriangleList, TriangleStrip };
	enum class PolygonFillMode { None = 0, Point, Line, Fill };

	struct PipelineConfiguration
	{
		std::shared_ptr<Shader> VertexShader = nullptr;
		std::shared_ptr<Shader> FragmentShader = nullptr;
		TopologyType Topology = TopologyType::TriangleList;
		PolygonFillMode PolygonFillMode = PolygonFillMode::Fill;
		float LineWidth = 1.0f;
		// TODO:
		// ColorBlendingMode
		// Uniforms & Push constants
	};
}