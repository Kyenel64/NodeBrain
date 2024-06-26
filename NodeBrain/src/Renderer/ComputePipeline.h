#pragma once

#include "Renderer/RenderContext.h"
#include "Renderer/Shader.h"
#include "Renderer/Image.h"
#include "Renderer/DescriptorSet.h"

namespace NodeBrain
{
	struct ComputePipelineConfiguration
	{
		std::shared_ptr<Shader> ComputeShader;
		std::shared_ptr<Image> TargetImage = nullptr;

		void AddDescriptorSet(const std::shared_ptr<DescriptorSet>& descriptorSet, uint32_t setIndex)
		{
			DescriptorSets.insert(DescriptorSets.begin() + setIndex, descriptorSet);
		}

		[[nodiscard]] const std::vector<std::shared_ptr<DescriptorSet>>& GetDescriptorSets() const { return DescriptorSets; }

	private:
		std::vector<std::shared_ptr<DescriptorSet>> DescriptorSets;
	};

	class ComputePipeline
	{
	public:
		virtual ~ComputePipeline() = default;

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) = 0;
		virtual void SetTargetImage(std::shared_ptr<Image> targetImage) = 0;

		virtual void BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet) = 0;

		[[nodiscard]] virtual std::shared_ptr<Image> GetTargetImage() const = 0;

		static std::shared_ptr<ComputePipeline> Create(RenderContext& context, const ComputePipelineConfiguration& configuration);
	};
}