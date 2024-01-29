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
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnUpdateGUI() override;

	private:

	};
}