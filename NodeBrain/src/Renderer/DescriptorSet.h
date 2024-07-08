#pragma once

#include "Renderer/RenderContext.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/Texture2D.h"

namespace NodeBrain
{
	enum class BindingType { StorageImage, UniformBuffer, ImageSampler };

	struct LayoutBinding
	{
		BindingType Type;
		uint32_t Binding;
		uint32_t Count;
	};

	class DescriptorSet
	{
	public:
		virtual ~DescriptorSet() = default;

		virtual void WriteBuffer(const std::shared_ptr<UniformBuffer>& buffer, uint32_t binding) = 0;
		virtual void WriteImage(const std::shared_ptr<Texture2D>& image, uint32_t binding) = 0;
		virtual void WriteSampler(const std::shared_ptr<Texture2D>& image, uint32_t binding) = 0;
		virtual void WriteSamplers(const std::vector<std::shared_ptr<Texture2D>>& textures, uint32_t binding) = 0;

		static std::shared_ptr<DescriptorSet> Create(RenderContext& context, const std::vector<LayoutBinding>& layout);
	};
}
