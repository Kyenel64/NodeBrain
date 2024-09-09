#pragma once

#include "Renderer/RenderContext.h"
#include "Renderer/UniformBuffer.h"
#include "Renderer/Texture2D.h"

namespace NodeBrain
{
	enum class BindingType
	{
		Sampler = 0,
		ImageSampler,
		SampledImage,
		StorageImage,
		UniformTexelBuffer,
		StorageTexelBuffer,
		UniformBuffer,
		StorageBuffer,
		UniformBufferDynamic,
		StorageBufferDynamic,
		InputAttachment,
		AccelerationStructure
	};

	struct UniformVariable
	{
		std::string Name;
		uint32_t Size;
		uint32_t Offset;
	};

	struct LayoutBinding
	{
		std::string Name;
		BindingType Type;
		uint32_t Binding;
		uint32_t Set;
		uint32_t Count;

		// Temporary used to store variable information in uniform buffer. TODO: Use vulkan reflect.
		std::vector<UniformVariable> UniformVariables;
	};

	class DescriptorSet
	{
	public:
		virtual ~DescriptorSet() = default;

		virtual void WriteBuffer(const std::shared_ptr<UniformBuffer>& ubo, uint32_t binding, uint32_t size, uint32_t offset = 0) = 0;
		virtual void WriteImage(const std::shared_ptr<Texture2D>& texture, uint32_t binding) = 0;
		virtual void WriteSampler(const std::shared_ptr<Texture2D>& texture, uint32_t binding) = 0;
		virtual void WriteSamplers(const std::vector<std::shared_ptr<Texture2D>>& textures, uint32_t binding) = 0;

		[[nodiscard]] virtual const std::vector<LayoutBinding>& GetLayout() const = 0;

		static std::shared_ptr<DescriptorSet> Create(RenderContext& context, const std::vector<LayoutBinding>& layout);
	};
}
