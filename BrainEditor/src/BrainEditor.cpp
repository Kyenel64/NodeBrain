#include "BrainEditor.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ImGui/imgui.h>

namespace NodeBrain
{
	void BrainEditor::OnAttach()
	{
		NB_INFO("Attached Brain Editor layer");

		// Demonstrating renderer backend
		m_GradientShader = Shader::Create("Assets/Shaders/Compiled/gradient.comp.spv", ShaderType::Compute);
		m_GradientShader->SetLayout({ { BindingType::StorageImage, 1} });
		m_GradientPipeline = ComputePipeline::Create(m_GradientShader);


		ImageConfiguration config = {};
		config.Width = 1280 / 2;
		config.Height = 720 / 2;
		config.Format = ImageFormat::RGBA16;
		m_TargetImage = Image::Create(config);

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
		if (m_ShaderIndex == 0)
			Renderer::ProcessGradientCompute();
		else if (m_ShaderIndex == 1)
		{
			// Demonstrate renderer backend
			//m_GradientPipeline->SetTargetImage(m_TargetImage);
			Renderer::BeginComputePass(m_GradientPipeline);
			uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
			uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
			Renderer::DispatchCompute(groupX, groupY, 1);
			Renderer::EndComputePass(m_GradientPipeline);
		}

		glm::mat4 transform  = glm::translate(glm::mat4(1.0f), { 0.5f, -0.5f, 0.5f }) * glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 0.5f }) * glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), { 1.0f, 1.0f, 0.0f });
		glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), { -0.5f, 0.5f, 0.0f }) * glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 0.5f });
			
		Renderer::BeginScene(m_TargetImage);
		Renderer::SubmitQuad(transform, { 1.0f, 0.0f, 0.0f, 1.0f });
		Renderer::SubmitQuad(transform1, { 0.0f, 1.0f, 0.0f, 1.0f });
		Renderer::EndScene();
	}

	void BrainEditor::OnUpdateGUI()
	{
		ImGui::ShowDemoWindow();

		ImGui::Begin("TestWindow");

		ImGui::SliderInt("Switch Shader", &m_ShaderIndex, 0, 1);

		ImGui::Image((ImTextureID)m_TargetImage->GetAddress(), { 1280 / 2, 720 / 2 });

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