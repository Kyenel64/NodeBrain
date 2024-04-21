#include "BrainEditor.h"

#include <iostream>

#include <ImGui/imgui.h>

namespace NodeBrain
{
	void BrainEditor::OnAttach()
	{
		NB_INFO("Attached Brain Editor layer");

		// Demonstrating renderer backend
		m_GradientShader = Shader::Create("Assets/Shaders/Compiled/gradient2.comp.spv", ShaderType::Compute);
		m_GradientShader->SetLayout({ { BindingType::StorageImage, 1} });
		m_GradientPipeline = ComputePipeline::Create(m_GradientShader);
		Renderer::TempUpdateImage(m_GradientShader); // Temp
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

		if (m_SwitchShader)
			Renderer::ProcessTestCompute();
		else
		{
			// Renderer backend
			Renderer::BeginComputePass(m_GradientPipeline);
			uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
			uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
			Renderer::DispatchCompute(groupX, groupY, 1);
			Renderer::EndComputePass();
		}
	}

	void BrainEditor::OnUpdateGUI()
	{
		ImGui::ShowDemoWindow();

		ImGui::Begin("TestWindow");

		ImGui::Checkbox("Switch Shader", &m_SwitchShader);

		ImGui::End();
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