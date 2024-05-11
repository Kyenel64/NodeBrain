#pragma once

#include <filesystem>

#include "Renderer/Image.h"
#include "Renderer/UniformBuffer.h"

namespace NodeBrain
{
	enum class ShaderType { Vertex, Fragment, Compute };
	enum class BindingType { StorageImage, UniformBuffer };

	struct LayoutBinding
	{
		BindingType Type;
		uint32_t Binding;
		std::shared_ptr<Image> BindingImage = nullptr;
		std::shared_ptr<UniformBuffer> BindingBuffer = nullptr;
	};



	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void SetLayout(const std::vector<LayoutBinding> layout) = 0;
		virtual void SetPushConstantLayout(uint32_t size, uint32_t offset) = 0;

		virtual const std::filesystem::path& GetShaderPath() const = 0;

		static std::shared_ptr<Shader> Create(const std::filesystem::path& path, ShaderType shaderType);
	};
}