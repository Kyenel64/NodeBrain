#pragma once

#include <glm/glm.hpp>
#include <ImGui/imgui.h>

#include <NodeBrain/NodeBrain.h>

#include "Panels/EntityGraphPanel.h"
#include "Panels/SceneGraphPanel.h"

namespace NodeBrain
{
	class BrainEditor
	{
	public:
		explicit BrainEditor(Renderer& renderer);
		~BrainEditor() = default;

		void OnEvent(Event& event);
		void OnUpdate(float deltaTime);
		void OnUpdateGUI();

	private:
		void OnKeyPressed(KeyPressedEvent& event);
		void OnMousePressed(MousePressedEvent& event);

		void DrawViewportWindow();

	private:
		Renderer& m_Renderer;
		RendererAPI& m_RendererAPI;
		RenderContext& m_Context;
		Window& m_Window;

		SceneGraphPanel m_SceneGraphPanel;
		EntityGraphPanel m_EntityGraphPanel;

		std::shared_ptr<EditorCamera> m_EditorCamera;
		std::shared_ptr<Scene> m_EditorScene;

		std::shared_ptr<Framebuffer> m_ViewportFramebuffer;
		ImVec2 m_ViewportSize = { 0.0f, 0.0f };

		Entity m_SelectedEntity;

		// Temp
		std::shared_ptr<Texture2D> m_TestTexture;
		std::shared_ptr<Texture2D> m_BrickTexture;

		std::shared_ptr<Mesh> m_TestMesh;
	};
}
