#include "BrainEditor.h"

#include <iostream>

namespace NodeBrain
{
	void BrainEditor::OnAttach()
	{
		NB_INFO("Attached Brain Editor layer");
	}

	void BrainEditor::OnDetach()
	{

	}

	void BrainEditor::OnEvent(Event& event)
	{
		event.AttachEventFunction<KeyPressedEvent>(std::bind(&BrainEditor::OnKeyPressed, this, std::placeholders::_1));
		event.AttachEventFunction<MouseMovedEvent>(std::bind(&BrainEditor::OnMouseMoved, this, std::placeholders::_1));
	}

	void BrainEditor::OnUpdate(float deltaTime)
	{
		
	}

	void BrainEditor::OnUpdateGUI()
	{

	}

	void BrainEditor::OnKeyPressed(KeyPressedEvent& event)
	{
		NB_INFO(event.GetKey());
	}

	void BrainEditor::OnMouseMoved(MouseMovedEvent& event)
	{
		NB_INFO("Mouse Position: {0}, {1}", event.GetXPos(), event.GetYPos());
	}
}