#pragma once

#include "Renderer/RenderContext.h"

namespace NodeBrain
{
	enum class ImageFormat { None = 0, RGBA8, RGBA16 };

	struct ImageConfiguration
	{
		uint32_t Width, Height;
		ImageFormat Format;
	};

	class Image
	{
	public:
		virtual ~Image() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		// Pointer address is equal to VkDescriptorSet. Can only be accessed by ImGui.
		virtual uint64_t GetAddress() = 0;
		[[nodiscard]] virtual const ImageConfiguration& GetConfiguration() const = 0;

		static std::shared_ptr<Image> Create(RenderContext& context, const ImageConfiguration& configuration);
	};
}