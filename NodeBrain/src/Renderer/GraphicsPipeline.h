#pragma once

#include "Renderer/RenderContext.h"
#include "Renderer/Shader.h"
#include "Renderer/Image.h"
#include "Renderer/DescriptorSet.h"
#include "Renderer/Framebuffer.h"

namespace NodeBrain
{
	enum class TopologyType { PointList = 0, LineList, LineStrip, TriangleList, TriangleStrip };
	enum class PolygonFillMode { Point = 0, Line, Fill };

	struct GraphicsPipelineConfiguration
	{
		std::shared_ptr<Shader> VertexShader;
		std::shared_ptr<Shader> FragmentShader;

		std::shared_ptr<Framebuffer> TargetFramebuffer = nullptr;

		TopologyType Topology = TopologyType::TriangleList;
		PolygonFillMode FillMode = PolygonFillMode::Fill;
		float LineWidth = 1.0f;
		// TODO:
		// ColorBlendingMode


		void AddDescriptorSet(const std::shared_ptr<DescriptorSet>& descriptorSet, uint32_t setIndex)
		{
			DescriptorSets.insert(DescriptorSets.begin() + setIndex, descriptorSet);
		}

		[[nodiscard]] const std::vector<std::shared_ptr<DescriptorSet>>& GetDescriptorSets() const { return DescriptorSets; }

	private:
		std::vector<std::shared_ptr<DescriptorSet>> DescriptorSets;
	};

	class GraphicsPipeline
	{
	public:
		virtual ~GraphicsPipeline() = default;

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) = 0;
		virtual void SetTargetFramebuffer(std::shared_ptr<Framebuffer> targetFramebuffer) = 0;

		virtual void BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet) = 0;

		[[nodiscard]] virtual std::shared_ptr<Framebuffer> GetTargetFramebuffer() const = 0;

		static std::shared_ptr<GraphicsPipeline> Create(RenderContext& context, const GraphicsPipelineConfiguration& configuration);
	};
}