#pragma once

#include <functional>
#include <queue>

#include <ImGui/imgui.h>
#include <glm/glm.hpp>

#include <NodeBrain/NodeBrain.h>

namespace NodeBrain
{
	struct OutputPortUI
	{
		std::string Name;
		ImVec2 PortPos;

		explicit OutputPortUI(std::string name, const ImVec2& pos = { 0.0f, 0.0f })
			: Name(std::move(name)), PortPos(pos) {}
	};

	struct InputPortUI
	{
		std::string Name;
		ImVec2 PortPos;

		explicit InputPortUI(std::string name, const ImVec2& pos = { 0.0f, 0.0f })
			: Name(std::move(name)), PortPos(pos) {}
	};

	struct NodeUI
	{
		std::string NodeName = "Node";
		ImVec4 NodeColor = { 0.3f, 0.3f, 0.3f, 1.0f };
		ImVec2 Size = { 100.0f, 60.0f };
		ImVec2 Pos = { 0.0f, 0.0f };

		NodeUI(std::string name, const ImVec4 nodeColor, const ImVec2& size, const ImVec2& pos)
			: NodeName(std::move(name)), NodeColor(nodeColor), Size(size), Pos(pos) {}
	};


	using NodeToUI = std::unordered_map<Node*, NodeUI>;
	using InputPortToUI = std::unordered_map<InputPort*, InputPortUI>;
	using OutputPortToUI = std::unordered_map<OutputPort*, OutputPortUI>;

	class EntityGraphPanel
	{
	public:
		EntityGraphPanel() = default;
		~EntityGraphPanel() = default;

		void Draw(const std::shared_ptr<Scene>& activeScene, Entity selectedEntity);

	private:
		// Main styling for nodes. Optional uiFunction for nodes with unique styling elements.
		void DrawNodeUI(Node& node, const std::function<void()>& uiFunction = nullptr);

		void ProcessAddNodePopup();
		void ProcessNodeMenuPopup();

	private:
		std::unordered_map<Entity, NodeToUI> m_NodeUIs;
		std::unordered_map<Entity, InputPortToUI > m_InputPortUIs;
		std::unordered_map<Entity, OutputPortToUI > m_OutputPortUIs;

		std::queue<Node*> m_DeletionQueue;

		ImVec2 m_GridOrigin = { 0.0f, 0.0f };
		ImVec2 m_EntityGraphPan = { 0.0f, 0.0f };

		std::shared_ptr<Scene> m_ActiveScene;
		Entity m_SelectedEntity;
		EntityGraph* m_EntityGraph = nullptr;
		Node* m_SelectedNode = nullptr;
		OutputPort* m_SelectedOutputPort = nullptr;
		bool m_AddingLink = false;

	};
}