#include "NBpch.h"
#include "Material.h"

#include <glm/gtc/type_ptr.hpp>

namespace NodeBrain
{
    Material::Material(RenderContext& context, const std::shared_ptr<GraphicsPipeline>& pipeline)
        : m_Context(context), m_Pipeline(pipeline)
    {
        // Per object will always be at set index 0.
        std::vector<LayoutBinding> layout;
        for (auto& binding : pipeline->GetConfiguration().VertexShader->GetLayout())
        {
            if (binding.Set == 0)
                layout.push_back(binding);
        }

        for (auto& binding : pipeline->GetConfiguration().FragmentShader->GetLayout())
        {
            // Check for duplicates from vertex shader
            bool duplicate = false;
            for (auto& existingBinding : layout)
            {
                if (existingBinding.Binding == binding.Binding)
                    duplicate = true;
            }
            if (!duplicate && binding.Set == 0)
                layout.push_back(binding);
        }

        // TODO: Should create a descriptor set from the pipeline descriptor layout. Currently creating a layout for each material.
        m_DescriptorSet = DescriptorSet::Create(m_Context, layout);

        // Calculate total size of material uniform buffer. Material uniform will always be at binding index 1.
        uint32_t size = 0;
        for (auto& var : m_DescriptorSet->GetLayout()[1].UniformVariables)
            size += var.Size;

        if (size)
            m_UBO = UniformBuffer::Create(m_Context, nullptr, size);
    }

    void Material::SetData(const std::string& varName, const glm::vec4& value)
    {
        bool found = false;
        for (const auto& var : m_DescriptorSet->GetLayout()[1].UniformVariables)
        {
            if (var.Name == varName)
            {
                m_UBO->SetData(glm::value_ptr(value), var.Size, var.Offset);
                m_DescriptorSet->WriteBuffer(m_UBO, 1, var.Size, var.Offset);
                found = true;
            }
        }

        NB_ASSERT(found, "Could not find variable of given name within material shader.");
    }

    void Material::SetData(const std::string& varName, const std::shared_ptr<Texture2D>& texture)
    {
        bool found = false;
        for (const auto& binding : m_DescriptorSet->GetLayout())
        {
            if (binding.Name == varName)
            {
                m_DescriptorSet->WriteSampler(texture, binding.Binding);
                found = true;
            }
        }

        NB_ASSERT(found, "Could not find sampler of given name within material shader");
    }
}
