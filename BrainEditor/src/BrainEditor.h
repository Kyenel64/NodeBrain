#pragma once

#include <glm/glm.hpp>
#include <ImGui/imgui.h>

#include <NodeBrain/NodeBrain.h>

#include "Panels/EntityGraphPanel.h"
#include "Panels/SceneGraphPanel.h"

namespace NodeBrain
{
	class BrainEditor : public Layer
	{
	public:
		BrainEditor(Renderer* renderer)
			: m_Renderer(renderer), m_RendererAPI(renderer->GetAPI()), m_Context(renderer->GetContext()), m_Window(renderer->GetContext()->GetWindow()) {}
		~BrainEditor() override = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& event) override;
		void OnUpdate(float deltaTime) override;
		void OnUpdateGUI() override;

	private:
		void OnKeyPressed(KeyPressedEvent& event);
		void OnMousePressed(MousePressedEvent& event);

		void DrawViewportWindow();

	private:
		Renderer* m_Renderer;
		RendererAPI* m_RendererAPI;
		RenderContext* m_Context;
		Window* m_Window;

		SceneGraphPanel m_SceneGraphPanel;
		EntityGraphPanel m_EntityGraphPanel;

		std::shared_ptr<EditorCamera> m_EditorCamera;
		std::shared_ptr<Scene> m_EditorScene;

		std::shared_ptr<Image> m_ViewportImage;
		ImVec2 m_ViewportSize = { 0.0f, 0.0f };

		Entity m_SelectedEntity;
	};
}