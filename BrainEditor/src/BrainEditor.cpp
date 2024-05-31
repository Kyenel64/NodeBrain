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
		ImageConfiguration config = {};
		config.Width = 1280 / 2;
		config.Height = 720 / 2;
		config.Format = ImageFormat::RGBA16;
		m_TargetImage = Image::Create(m_Context, config);

		m_GradientUB = UniformBuffer::Create(m_Context, nullptr, sizeof(GradientData));

		m_GradientDescriptorSet = DescriptorSet::Create(m_Context, { { BindingType::StorageImage, 0 }, { BindingType::UniformBuffer, 1} });
		m_GradientDescriptorSet->WriteImage(m_TargetImage, 0);
		m_GradientDescriptorSet->WriteBuffer(m_GradientUB, 1);

		m_GradientShader = Shader::Create(m_Context, "Assets/Shaders/Compiled/gradient.comp.spv", ShaderType::Compute);
		ComputePipelineConfiguration gradientConfig = {};
		gradientConfig.ComputeShader = m_GradientShader;
		gradientConfig.TargetImage = m_TargetImage;
		gradientConfig.AddDescriptorSet(m_GradientDescriptorSet, 0);
		m_GradientPipeline = ComputePipeline::Create(m_Context, gradientConfig);

		m_EditorCamera = std::make_shared<EditorCamera>(45.0f, m_Window->GetWidth() / m_Window->GetHeight(), 0.01f, 1000.0f);

		m_EditorScene = std::make_shared<Scene>(m_Renderer);
		Entity testEntity = m_EditorScene->CreateEntity();
		m_EditorScene->AddComponent<TransformComponent>(testEntity);
	}

	void BrainEditor::OnDetach()
	{

	}

	void BrainEditor::OnEvent(Event& event)
	{
		event.AttachEventFunction<KeyPressedEvent>([this](KeyPressedEvent& event) { OnKeyPressed(event); });
		event.AttachEventFunction<MousePressedEvent>([this](MousePressedEvent& event) { OnMousePressed(event); });
	}

	void BrainEditor::OnUpdate(float deltaTime)
	{
		m_EditorCamera->OnUpdate(deltaTime);
		m_EditorScene->OnUpdate(m_EditorCamera);
	}

	void BrainEditor::OnUpdateGUI()
	{
		ImGui::ShowDemoWindow();

		ImGui::Begin("TestWindow");

		ImGui::SliderInt("Switch Shader", &m_ShaderIndex, 0, 1);

		ImGui::Image((ImTextureID)m_TargetImage->GetAddress(), { (float)m_TargetImage->GetConfiguration().Width, (float)m_TargetImage->GetConfiguration().Height});

		ImGui::End();
	}

	void BrainEditor::OnKeyPressed(KeyPressedEvent& event)
	{
		NB_INFO((int)event.GetKey());
	}

	void BrainEditor::OnMousePressed(MousePressedEvent& event)
	{
		NB_INFO("Mouse Position: {0}, {1}", Input::GetMousePosition().x, Input::GetMousePosition().y);
	}
}