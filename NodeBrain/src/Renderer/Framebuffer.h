#pragma once

#include "Renderer/RenderPass.h"

namespace NodeBrain
{
	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		static std::shared_ptr<Framebuffer> Create(std::shared_ptr<RenderPass> renderPass);
	};
}