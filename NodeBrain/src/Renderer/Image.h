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

		// Pointer address is equal to VkDescriptorSet. Can only be accessed by ImGui.
		virtual uint64_t GetAddress() = 0;
		virtual const ImageConfiguration& GetConfiguration() const = 0; 

		static std::shared_ptr<Image> Create(RenderContext* context, const ImageConfiguration& configuration);
	};
}