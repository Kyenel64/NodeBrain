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
		void OnMouseMoved(MouseMovedEvent& event);
	};
}