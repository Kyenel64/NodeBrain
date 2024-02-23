#pragma once

#include <vulkan/vulkan.h>

#include "GAPI/Vulkan/VulkanShader.h"
#include "Renderer/GraphicsPipeline.h"

namespace NodeBrain
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	public:
		VulkanGraphicsPipeline(std::shared_ptr<Shader> vertShader, std::shared_ptr<Shader> fragShader);
		virtual ~VulkanGraphicsPipeline() = default;
	private:
		void Init();

	private:
		std::shared_ptr<VulkanShader> m_VertexShader;
		std::shared_ptr<VulkanShader> m_FragmentShader;
	};
}