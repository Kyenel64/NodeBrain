#include "NBpch.h"
#include "VulkanShader.h"

#include <SPIRV-Reflect/spirv_reflect.h>

#include "Utils/FileUtils.h"

namespace NodeBrain
{
	namespace Utils
	{
		static BindingType SpvBindingTypeToBindingType(SpvReflectDescriptorType type)
		{
			switch (type)
			{
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER: return BindingType::Sampler;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return BindingType::ImageSampler;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return BindingType::SampledImage;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE: return BindingType::StorageImage;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: return BindingType::UniformTexelBuffer;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: return BindingType::StorageTexelBuffer;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return BindingType::UniformBuffer;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER: return BindingType::StorageBuffer;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: return BindingType::UniformBufferDynamic;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return BindingType::StorageBufferDynamic;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: return BindingType::InputAttachment;
			case SpvReflectDescriptorType::SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR: return BindingType::AccelerationStructure;
			}
		}

		static InputFormat SpvFormatToInputFormat(SpvReflectFormat format)
		{
			switch (format)
			{
			case SPV_REFLECT_FORMAT_R32_SFLOAT: return InputFormat::R32;
			case SPV_REFLECT_FORMAT_R32G32_SFLOAT: return InputFormat::R32G32;
			case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT: return InputFormat::R32G32B32;
			case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT: return InputFormat::R32G32B32A32;
			default: return InputFormat::None;
			}
		}
	}


	VulkanShader::VulkanShader(VulkanRenderContext& context, const std::filesystem::path& path)
		: m_Context(context), m_ShaderPath(path)
	{
		NB_PROFILE_FN();

		NB_ASSERT(path.extension() == ".spv", "Path must point to a valid .spv file");

		// Set shader type
		if (path.stem().extension() == ".vert")
			m_ShaderType = ShaderType::Vertex;
		else if (path.stem().extension() == ".frag")
			m_ShaderType = ShaderType::Fragment;

		// Create shader module
		const std::vector<char> buffer = Utils::ReadFile(m_ShaderPath);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = buffer.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
		VK_CHECK(vkCreateShaderModule(m_Context.GetVkDevice(), &createInfo, nullptr, &m_VkShaderModule));

		// Gather reflection data
		SpvReflectShaderModule module;
		SpvReflectResult result = spvReflectCreateShaderModule(buffer.size(), buffer.data(), &module);
		NB_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, result);
		Reflect(module);
		spvReflectDestroyShaderModule(&module);

		NB_INFO("Created shader module of size: {0}", buffer.size());
	}

	VulkanShader::~VulkanShader()
	{
		NB_PROFILE_FN();

		m_Context.WaitForGPU();

		vkDestroyShaderModule(m_Context.GetVkDevice(), m_VkShaderModule, nullptr);
		m_VkShaderModule = VK_NULL_HANDLE;
	}

	void VulkanShader::Reflect(const SpvReflectShaderModule& module)
	{
		NB_PROFILE_FN();

		// Input variables
		uint32_t layoutCount = 0;
		SpvReflectResult result = spvReflectEnumerateInputVariables(&module, &layoutCount, nullptr);
		NB_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, result);
		SpvReflectInterfaceVariable** inputs = (SpvReflectInterfaceVariable**)malloc(layoutCount * sizeof(SpvReflectInterfaceVariable*));
		result = spvReflectEnumerateInputVariables(&module, &layoutCount, inputs);
		NB_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, result);

		uint32_t offset = 0;
		for (size_t i = 0; i < layoutCount; i++)
		{
			const SpvReflectInterfaceVariable& inputVariable = *(inputs[i]);
			InputVariable variable;
			variable.Name = inputVariable.name;
			variable.Location = inputVariable.location;
			variable.Format = Utils::SpvFormatToInputFormat(inputVariable.format);
			variable.Offset = offset;

			switch (variable.Format)
			{
			case InputFormat::R32: offset += 4; break;
			case InputFormat::R32G32: offset += 8; break;
			case InputFormat::R32G32B32: offset += 12; break;
			case InputFormat::R32G32B32A32: offset += 16; break;
			default: break;
			}

			m_InputVariables.emplace_back(variable);
		}

		delete[] inputs;


		// Descriptors
		uint32_t bindingCount = 0;
		result = spvReflectEnumerateDescriptorBindings(&module, &bindingCount, nullptr);
		NB_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, result);
		SpvReflectDescriptorBinding** bindings = (SpvReflectDescriptorBinding**)malloc(bindingCount * sizeof(SpvReflectDescriptorBinding*));
		result = spvReflectEnumerateDescriptorBindings(&module, &bindingCount, bindings);
		NB_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS, result);

		for (size_t i = 0; i < bindingCount; i++)
		{
			const SpvReflectDescriptorBinding& spvBinding = *(bindings[i]);
			LayoutBinding binding = {};
			binding.Binding = spvBinding.binding;
			binding.Set = spvBinding.set;
			binding.Count = spvBinding.count;
			binding.Name = spvBinding.name;
			binding.Type = Utils::SpvBindingTypeToBindingType(spvBinding.descriptor_type);

			for (size_t j = 0; j < spvBinding.block.member_count; j++)
			{
				const SpvReflectBlockVariable& variable = spvBinding.block.members[j];
				binding.UniformVariables.push_back({ variable.name, variable.size, variable.offset });
			}

			m_Layout.emplace_back(binding);
		}

		delete[] bindings;
	}
}
