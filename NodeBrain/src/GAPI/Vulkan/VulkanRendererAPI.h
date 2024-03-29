#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/RendererAPI.h"

namespace NodeBrain
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		VulkanRendererAPI();
		virtual ~VulkanRendererAPI();

	private:
		void Init();

	};
}