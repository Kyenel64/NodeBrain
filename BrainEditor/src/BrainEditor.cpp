#include "BrainEditor.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ImGui/imgui.h>
#include <GLFW/glfw3.h>

namespace NodeBrain
{
	void BrainEditor::OnAttach()
	{
		NB_INFO("Attached Brain Editor layer");

		// Demonstrating renderer backend
		ImageConfiguration config = {};
		config.Width = 1280 / 2;
		config.Height = 720 / 2;
		config.Format = ImageFormat::RGBA16;
		m_TargetImage = Image::Create(config);

		m_GradientUB = UniformBuffer::Create(nullptr, sizeof(GradientData));

		m_GradientDescriptorSet = DescriptorSet::Create({ { BindingType::StorageImage, 0 }, { BindingType::UniformBuffer, 1} });
		m_GradientDescriptorSet->WriteImage(m_TargetImage, 0);
		m_GradientDescriptorSet->WriteBuffer(m_GradientUB, 1);

		m_GradientShader = Shader::Create("Assets/Shaders/Compiled/gradient.comp.spv", ShaderType::Compute);
		ComputePipelineConfiguration gradientConfig = {};
		gradientConfig.ComputeShader = m_GradientShader;
		gradientConfig.TargetImage = m_TargetImage;
		gradientConfig.AddDescriptorSet(m_GradientDescriptorSet, 0);
		m_GradientPipeline = ComputePipeline::Create(gradientConfig);
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
		const float radius = 1.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;

		if (m_ShaderIndex == 0)
		{
			glm::mat4 transform  = glm::translate(glm::mat4(1.0f), { camX, -0.5f, 0.5f }) * glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 0.5f }) * glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), { 1.0f, 1.0f, 0.0f });
			glm::mat4 transform1 = glm::translate(glm::mat4(1.0f), { -0.5f, 0.5f, 0.0f }) * glm::scale(glm::mat4(1.0f), { 0.5f, 0.5f, 0.5f });

			Renderer::BeginScene();
			Renderer::SubmitQuad(transform, { 1.0f, 0.0f, 0.0f, 1.0f });
			Renderer::SubmitQuad(transform1, { 0.0f, 1.0f, 0.0f, 1.0f });
			Renderer::EndScene();
		}
		else if (m_ShaderIndex == 1)
		{
			Renderer::ProcessGradientCompute();
		}
		else if (m_ShaderIndex == 2)
		{
			m_GradientBuffer.Color1 = { camX, 0.0f, camZ, 1.0f };
			m_GradientUB->SetData(&m_GradientBuffer, sizeof(GradientData));
			m_GradientPipeline->BindDescriptorSet(m_GradientDescriptorSet);
			Renderer::BeginComputePass(m_GradientPipeline);
			uint32_t groupX = App::Get()->GetWindow().GetWidth() / 16;
			uint32_t groupY = App::Get()->GetWindow().GetHeight() / 16;
			Renderer::DispatchCompute(groupX, groupY, 1);
			Renderer::EndComputePass(m_GradientPipeline);
		}
	}

	void BrainEditor::OnUpdateGUI()
	{
		ImGui::ShowDemoWindow();

		ImGui::Begin("TestWindow");

		ImGui::SliderInt("Switch Shader", &m_ShaderIndex, 0, 2);

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