#pragma once

#include "Renderer/Shader.h"

namespace NodeBrain
{
	struct ComputePipelineConfiguration
	{
		std::shared_ptr<Shader> ComputeShader;
		std::shared_ptr<Image> TargetImage = nullptr;

		void AddDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet, uint32_t setIndex)
		{
			DescriptorSets.insert(DescriptorSets.begin() + setIndex, descriptorSet);
		}

		const std::vector<std::shared_ptr<DescriptorSet>>& GetDescriptorSets() const { return DescriptorSets; }

	private:
		std::vector<std::shared_ptr<DescriptorSet>> DescriptorSets;
	};

	class ComputePipeline
	{
	public:
		virtual ~ComputePipeline() = default;

		virtual void SetPushConstantData(const void* buffer, uint32_t size, uint32_t offset) = 0;
		virtual void SetTargetImage(std::shared_ptr<Image> targetImage) = 0;

		virtual void BindDescriptorSet(std::shared_ptr<DescriptorSet> descriptorSet, uint32_t setIndex) = 0;

		virtual std::shared_ptr<Image> GetTargetImage() const = 0;

		static std::shared_ptr<ComputePipeline> Create(const ComputePipelineConfiguration& configuration);
	};
}