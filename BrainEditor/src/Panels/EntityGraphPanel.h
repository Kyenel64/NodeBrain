#pragma once

#include <functional>

#include <ImGui/imgui.h>
#include <glm/glm.hpp>

#include <NodeBrain/NodeBrain.h>

namespace NodeBrain
{
	struct OutputPortUI
	{
		OutputPort& OwnedOutputPort;
		ImVec2 PortPos = { 0.0f, 0.0f };
	};

	struct InputPortUI
	{
		InputPort& OwnedInputPort;
		OutputPortUI* LinkedOutputPortUI;
		ImVec2 PortPos = { 0.0f, 0.0f };
	};

	struct NodeUI
	{
		std::shared_ptr<Node> OwnedNode;
		std::vector<OutputPortUI> OutputPorts;
		std::vector<InputPortUI> InputPorts;

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
		void DrawNodeUI(NodeUI& node, std::function<void()> uiFunction = nullptr);

		void ProcessAddNodePopup();

	private:
		std::unordered_map<Entity, std::vector<NodeUI>> m_NodeUIs;

		ImVec2 m_GridOrigin = { 0.0f, 0.0f };
		ImVec2 m_EntityGraphPan = { 0.0f, 0.0f };

		Entity m_SelectedEntity;
		std::shared_ptr<Scene> m_ActiveScene;
		EntityGraph* m_EntityGraph = nullptr;

		NodeUI* m_SelectedNodeUI = nullptr;
		OutputPortUI* m_SelectedOutputPortUI = nullptr;
		bool m_AddingLink = false;
		bool m_HoveringNode = false;

	};
}