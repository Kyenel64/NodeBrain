#include "BrainEditor.h"

#include <iostream>

#include <ImGui/imgui.h>

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
		event.AttachEventFunction<MousePressedEvent>(std::bind(&BrainEditor::OnMousePressed, this, std::placeholders::_1));
	}

	void BrainEditor::OnUpdate(float deltaTime)
	{
		if (Input::IsKeyReleased(Key::T))
			NB_INFO("'T' released");
		if (Input::IsMouseButtonReleased(MouseButton::Left))
			NB_INFO("Left mousebutton released");

		// Pseudo code. Eventually used in scene class
		Renderer::BeginScene();
		//Renderer::SubmitMesh();
		Renderer::EndScene();
		Renderer::ProcessTestCompute();


		//Renderer::Begin(framebuffer);

		//Renderer::End();
	}

	void BrainEditor::OnUpdateGUI()
	{
		ImGui::ShowDemoWindow();
	}

	void BrainEditor::OnKeyPressed(KeyPressedEvent& event)
	{
		NB_INFO(event.GetKey());
	}

	void BrainEditor::OnMousePressed(MousePressedEvent& event)
	{
		NB_INFO("Mouse Position: {0}, {1}", Input::GetMousePosition().x, Input::GetMousePosition().y);
	}
}