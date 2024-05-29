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
		BrainEditor(Renderer* renderer)
			: m_Renderer(renderer), m_RendererAPI(renderer->GetAPI()),
			m_Context(renderer->GetContext()), m_Window(renderer->GetContext()->GetWindow()) {}
		~BrainEditor() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnUpdate(float deltaTime) override;
		void OnUpdateGUI() override;

	private:
		void OnKeyPressed(KeyPressedEvent& event);
		void OnMousePressed(MousePressedEvent& event);

	private:
		Renderer* m_Renderer;
		RendererAPI* m_RendererAPI;
		RenderContext* m_Context;
		Window* m_Window;

		int m_ShaderIndex = 0;

		std::shared_ptr<EditorCamera> m_EditorCamera;

		// Demo
		std::shared_ptr<Shader> m_GradientShader;
		std::shared_ptr<ComputePipeline> m_GradientPipeline;

		std::shared_ptr<Image> m_TargetImage;
		
		std::shared_ptr<UniformBuffer> m_GradientUB;
		GradientData m_GradientBuffer;

		std::shared_ptr<DescriptorSet> m_GradientDescriptorSet;
	};
}