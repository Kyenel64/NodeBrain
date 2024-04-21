#pragma once

#include <NodeBrain/NodeBrain.h>

namespace NodeBrain
{
	class BrainEditor : public Layer
	{
	public:
		BrainEditor() = default;
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
		bool m_SwitchShader = true;

		// Demo
		std::shared_ptr<Shader> m_GradientShader;
		std::shared_ptr<ComputePipeline> m_GradientPipeline;

	};
}