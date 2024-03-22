#pragma once

#include "Renderer/Image.h"
#include "Renderer/RenderPass.h"

namespace NodeBrain
{
	struct FramebufferAttachmentConfiguration
	{
		// ImageFormat
	};

	struct FramebufferConfiguration
	{
		uint32_t Width, Height;

		std::shared_ptr<Image> Image = nullptr;
		std::shared_ptr<RenderPass> RenderPass = nullptr;

		std::vector<FramebufferAttachmentConfiguration> Attachments;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual const FramebufferConfiguration& GetConfiguration() const = 0;

		static std::shared_ptr<Framebuffer> Create(const FramebufferConfiguration& configuration);
	};
}