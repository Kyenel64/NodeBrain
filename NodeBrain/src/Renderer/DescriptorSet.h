#pragma once

#include "Renderer/UniformBuffer.h"
#include "Renderer/Image.h"

namespace NodeBrain
{
	enum class BindingType { StorageImage, UniformBuffer };

	struct LayoutBinding
	{
		BindingType Type;
		uint32_t Binding;
	};

	class DescriptorSet
	{
	public:
		virtual ~DescriptorSet() = default;

		virtual void WriteBuffer(std::shared_ptr<UniformBuffer> buffer, uint32_t binding) = 0;
		virtual void WriteImage(std::shared_ptr<Image> image, uint32_t binding) = 0;

		static std::shared_ptr<DescriptorSet> Create(const std::vector<LayoutBinding>& layout);
	};
}