#include "BrainEditor.h"

namespace NodeBrain
{
	BrainEditor::BrainEditor(Renderer* renderer)
		: m_Renderer(renderer), m_RendererAPI(renderer->GetAPI()), m_Context(renderer->GetContext()), m_Window(renderer->GetContext()->GetWindow())
	{
		NB_INFO("Attached Brain Editor layer");

		ImageConfiguration config = {};
		config.Width = 1280 / 2;
		config.Height = 720 / 2;
		config.Format = ImageFormat::RGBA16;
		m_ViewportImage = Image::Create(m_Context, config);

		m_EditorCamera = std::make_shared<EditorCamera>(45.0f, m_ViewportImage->GetConfiguration().Width / m_ViewportImage->GetConfiguration().Height, 0.01f, 1000.0f);
		m_EditorScene = std::make_shared<Scene>(m_Renderer);
	}

	void BrainEditor::OnEvent(Event& event)
	{
		event.AttachEventFunction<KeyPressedEvent>([this](KeyPressedEvent& event) { OnKeyPressed(event); });
		event.AttachEventFunction<MousePressedEvent>([this](MousePressedEvent& event) { OnMousePressed(event); });
	}

	void BrainEditor::OnUpdate(float deltaTime)
	{
		// On Viewport resize
		if ((m_ViewportSize.x != 0.0f && m_ViewportSize.y != 0.0f) &&
				((float)m_ViewportImage->GetConfiguration().Width != m_ViewportSize.x || (float)m_ViewportImage->GetConfiguration().Height != m_ViewportSize.y))
		{
			NB_INFO("Resizing Viewport");
			m_ViewportImage->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_EditorCamera->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		m_EditorCamera->OnUpdate(deltaTime);
		m_EditorScene->OnEditorUpdate(m_EditorCamera, m_ViewportImage);
	}

	void BrainEditor::OnUpdateGUI()
	{
		// --- Dockspace ---
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollWithMouse;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		bool open = false;
		ImGui::Begin("Layout", &open, window_flags);

		ImGuiDockNodeFlags docknodeFlags = ImGuiDockNodeFlags_None;
		ImGuiID dockspace_id = ImGui::GetID("BrainEditorDockSpace");
		ImGui::DockSpace(dockspace_id, { 0.0f, 0.0f }, docknodeFlags);


		m_SelectedEntity = m_SceneGraphPanel.GetSelectedEntity();


		// --- Panels ---
		ImGui::ShowDemoWindow();
		DrawViewportWindow();
		m_SceneGraphPanel.Draw(m_EditorScene);
		m_EntityGraphPanel.Draw(m_EditorScene, m_SelectedEntity);

		ImGui::End();
	}

	void BrainEditor::DrawViewportWindow()
	{
		ImGui::Begin("Viewport");

		m_ViewportSize = ImGui::GetContentRegionAvail();

		ImGui::Image((ImTextureID)m_ViewportImage->GetAddress(), { (float)m_ViewportImage->GetConfiguration().Width, (float)m_ViewportImage->GetConfiguration().Height});

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