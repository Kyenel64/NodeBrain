#pragma once

#include "Renderer/RenderContext.h"
#include "Renderer/Image.h"

namespace NodeBrain
{
	struct FramebufferConfiguration
	{
		uint32_t Width, Height;
		ImageFormat Format;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		[[nodiscard]] virtual uint64_t GetAddress() = 0;
		[[nodiscard]] virtual const FramebufferConfiguration& GetConfiguration() const = 0;

		static std::shared_ptr<Framebuffer> Create(RenderContext& context, const FramebufferConfiguration& configuration);
	};
}