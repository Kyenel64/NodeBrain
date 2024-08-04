#pragma once

#include "Renderer/RenderContext.h"
#include "Renderer/GraphicsPipeline.h"
#include "Renderer/DescriptorSet.h"
#include "Renderer/UniformBuffer.h"

namespace NodeBrain
{
    class Material
    {
    public:
        Material(RenderContext& context, const std::shared_ptr<GraphicsPipeline>& pipeline);

        [[nodiscard]] std::shared_ptr<GraphicsPipeline> GetPipeline() const { return m_Pipeline; }

        void SetData(const std::string& varName, const glm::vec4& value);
        void SetData(const std::string& varName, const std::shared_ptr<Texture2D>& texture);

    private:
        RenderContext& m_Context;
        std::shared_ptr<GraphicsPipeline> m_Pipeline;
        std::shared_ptr<DescriptorSet> m_DescriptorSet;

        std::shared_ptr<UniformBuffer> m_UBO;
        std::vector<std::shared_ptr<Texture2D>> m_Textures;
    };
}
