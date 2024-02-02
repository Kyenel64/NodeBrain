#pragma once

#include <vulkan/vulkan.h>

#include "Renderer/RendererAPI.h"

namespace NodeBrain
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual ~VulkanRendererAPI();

		virtual void Init() override;

	private:
		VkResult CreateInstance();

	private:
		VkInstance m_VkInstance = nullptr;
	};
}