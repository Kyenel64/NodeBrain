#pragma once

#include <NodeBrain/NodeBrain.h>

namespace NodeBrain
{
	struct GradientData
	{
		glm::vec4 Color1;
	};

	class BrainEditor : public Layer
	{
	public:
		BrainEditor(Renderer* renderer) : m_Renderer(renderer), m_RendererAPI(renderer->GetAPI()), m_Context(renderer->GetContext()) {}
		~BrainEditor() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnUpdateGUI() override;

	private:
		void OnKeyPressed(KeyPressedEvent& event);
		void OnMousePressed(MousePressedEvent& event);

	private:
		Renderer* m_Renderer;
		RendererAPI* m_RendererAPI;
		RenderContext* m_Context;

		int m_ShaderIndex = 0;

		// Demo
		std::shared_ptr<Shader> m_GradientShader;
		std::shared_ptr<ComputePipeline> m_GradientPipeline;

		std::shared_ptr<Image> m_TargetImage;
		
		std::shared_ptr<UniformBuffer> m_GradientUB;
		GradientData m_GradientBuffer;

		std::shared_ptr<DescriptorSet> m_GradientDescriptorSet;
	};
}