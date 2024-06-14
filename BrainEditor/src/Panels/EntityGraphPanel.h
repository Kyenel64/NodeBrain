#pragma once

#include <functional>

#include <ImGui/imgui.h>
#include <glm/glm.hpp>

#include <NodeBrain/NodeBrain.h>

namespace NodeBrain
{
	struct OutputPortUI
	{
		ImVec2 PortPos = { 0.0f, 0.0f };
	};

	struct InputPortUI
	{
		ImVec2 PortPos = { 0.0f, 0.0f };
	};

	struct NodeUI
	{
		std::string NodeName = "Node";
		ImVec4 NodeColor = { 0.3f, 0.3f, 0.3f, 1.0f };
		ImVec2 Size = { 100.0f, 60.0f };
		ImVec2 Pos = { 0.0f, 0.0f };
	};



	class EntityGraphPanel
	{
	public:
		EntityGraphPanel();
		~EntityGraphPanel() = default;

		void Draw(std::shared_ptr<Scene> activeScene, Entity selectedEntity);

	private:
		// Main styling for nodes. Optional uiFunction for nodes with unique styling elements.
		void DrawNodeUI(const std::shared_ptr<Node>& node, std::function<void()> uiFunction = nullptr);

		void ProcessAddNodePopup();
		void ProcessNodeMenuPopup();

	private:
		std::unordered_map<NodeID, NodeUI> m_NodeUIs;
		std::unordered_map<const InputPort*, InputPortUI> m_InputPortUIs;
		std::unordered_map<const OutputPort*, OutputPortUI> m_OutputPortUIs;

		ImVec2 m_GridOrigin = { 0.0f, 0.0f };
		ImVec2 m_EntityGraphPan = { 0.0f, 0.0f };

		Entity m_SelectedEntity;
		std::shared_ptr<Scene> m_ActiveScene;
		EntityGraph* m_EntityGraph = nullptr;

		Node* m_SelectedNode = nullptr;
		OutputPort* m_SelectedOutputPort = nullptr;
		bool m_AddingLink = false;

	};
}