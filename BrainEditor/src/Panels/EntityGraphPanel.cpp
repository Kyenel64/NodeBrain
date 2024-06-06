#include "NBpch.h"
#include "EntityGraphPanel.h"

namespace NodeBrain
{
	namespace Utils
	{
		static float Distance(const ImVec2& p1, const ImVec2& p2)
		{
			float dx = p2.x - p1.x;
			float dy = p2.y - p1.y;

			return sqrt((dx * dx) + (dy * dy));
		}

		static NodeUI PopulateNodeUI(std::shared_ptr<Node> node, const std::string& name, NodeType type, const ImVec2& pos, const ImVec2& size, const ImVec4& color)
		{
			const float portSpacing = 30.0f;
			NodeUI nodeUI;
			for (size_t i = 0; i < node->InputCount(); i++)
				nodeUI.InputPorts.push_back({ node->GetInputPort(i), { pos.x, pos.y + (i * portSpacing)}});
			for (size_t i = 0; i < node->OutputCount(); i++)
				nodeUI.OutputPorts.push_back({ node->GetOutputPort(i), { pos.x + size.x, pos.y + (i * portSpacing)}});
			nodeUI.OwnedNode = node;
			nodeUI.NodeName = name;
			nodeUI.Type = type;
			nodeUI.Pos = pos;
			nodeUI.Size = size;
			nodeUI.NodeColor = color;
			return nodeUI;
		}
	}


	EntityGraphPanel::EntityGraphPanel()
	{

	}

	void EntityGraphPanel::Draw(std::shared_ptr<Scene> activeScene, Entity selectedEntity)
	{
		m_EntityGraph = &activeScene->GetEntityGraph(selectedEntity);
		m_SelectedEntity = selectedEntity;
		m_ActiveScene = activeScene;

		ImGui::Begin("Entity Graph");

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiIO& io = ImGui::GetIO();

		// Calculate grid origin in screen coordinates
		m_GridOrigin = ImGui::GetCursorScreenPos();
		if (!m_AddingLink && ImGui::IsWindowFocused() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			m_EntityGraphPan.x += io.MouseDelta.x;
			m_EntityGraphPan.y += io.MouseDelta.y;
		}
		m_GridOrigin = { m_GridOrigin.x + m_EntityGraphPan.x, m_GridOrigin.y + m_EntityGraphPan.y };
		drawList->AddCircle(m_GridOrigin, 5.0f, ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f}));


		// Draw a curve to mouse position if in the process of linking ports
		if (m_AddingLink && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			ImVec2 portScreenPos = { m_GridOrigin.x + m_SelectedOutputPortUI->PortPos.x, m_GridOrigin.y + m_SelectedOutputPortUI->PortPos.y };
			drawList->AddBezierCubic( portScreenPos, { ImGui::GetMousePos().x, portScreenPos.y },
					{ portScreenPos.x, ImGui::GetMousePos().y }, ImGui::GetMousePos(), ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 0.5f}), 1.0f);
		}

		// Draw all connected links
		for (auto& linkUI : m_LinkUIs[m_SelectedEntity])
		{
			ImVec2 outputPos = { m_GridOrigin.x + linkUI.OutputPort.PortPos.x, m_GridOrigin.y + linkUI.OutputPort.PortPos.y };
			ImVec2 inputPos = { m_GridOrigin.x + linkUI.InputPort.PortPos.x, m_GridOrigin.y + linkUI.InputPort.PortPos.y };
			drawList->AddBezierCubic( outputPos, { inputPos.x, outputPos.y }, { outputPos.x, inputPos.y }, inputPos,
					ImGui::GetColorU32(linkUI.LinkColor), linkUI.LineThickness);
		}

		// Draw all nodes in entity graph
		for (auto& nodeUI : m_NodeUIs[m_SelectedEntity])
		{
			switch (nodeUI.Type)
			{
				case NodeType::TransformComponent: DrawNodeUI(nodeUI, m_GridOrigin); break;
				case NodeType::Vec3: DrawNodeUI(nodeUI, m_GridOrigin); break;
				case NodeType::None: break;
			}
		}

		// "Add Node" Popup
		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup("Add Node");
		ProcessAddNodePopup();


		ImGui::End();
	}

	void EntityGraphPanel::DrawNodeUI(NodeUI& node, const ImVec2& gridOrigin, std::function<void()> uiFunction)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImVec2 nodeTopLeft = { gridOrigin.x + node.Pos.x, gridOrigin.y + node.Pos.y };
		ImVec2 nodeBotRight = { nodeTopLeft.x + node.Size.x, nodeTopLeft.y + node.Size.y };

		drawList->AddRectFilled(nodeTopLeft, nodeBotRight, ImGui::GetColorU32(node.NodeColor), 10.0f );
		drawList->AddText(nodeTopLeft, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 1.0f }), node.NodeName.c_str());

		// Draw Output Ports
		for (auto& outputPort : node.OutputPorts)
		{
			ImVec2 portScreenPos = {gridOrigin.x + outputPort.PortPos.x, gridOrigin.y + outputPort.PortPos.y};
			drawList->AddCircleFilled(portScreenPos, 5, ImGui::GetColorU32({1.0f, 0.0f, 0.0f, 1.0f}));

			// When port is clicked, begin adding a link curve.
			if (!m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 10.0f) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedOutputPortUI = &outputPort;
				m_AddingLink = true;
			}
		}

		// Draw Input Ports
		for (auto& inputPort : node.InputPorts)
		{
			ImVec2 portScreenPos = { gridOrigin.x + inputPort.PortPos.x, gridOrigin.y + inputPort.PortPos.y };
			drawList->AddCircleFilled(portScreenPos, 5, ImGui::GetColorU32({1.0f, 0.0f, 0.0f, 1.0f}));

			// When link connects to this port, create a link.
			if (m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 10.0f) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				LinkUI link = { *m_SelectedOutputPortUI, inputPort };
				m_EntityGraph->AddLink(m_SelectedOutputPortUI->OwnedOutputPort, inputPort.OwnedInputPort);
				m_LinkUIs[m_SelectedEntity].push_back(link);
				m_AddingLink = false;
			}

			// When input port is clicked, remove link to the output port. TODO: Implement
			if (!m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 10.0f) &&
				ImGui::IsMouseClicked(ImGuiMouseButton_Left) && inputPort.OwnedInputPort.LinkedOutputPort)
			{

			}
		}
	}

	void EntityGraphPanel::ProcessAddNodePopup()
	{
		if (ImGui::BeginPopup("Add Node"))
		{
			const ImVec2 addNodePos = { ImGui::GetMousePosOnOpeningCurrentPopup().x - m_GridOrigin.x, ImGui::GetMousePosOnOpeningCurrentPopup().y - m_GridOrigin.y };
			if (ImGui::MenuItem("Transform Component"))
			{
				std::shared_ptr<TransformComponentNode> node = m_EntityGraph->AddNode<TransformComponentNode>(m_ActiveScene->GetComponent<TransformComponent>(m_SelectedEntity));
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "TransformComponent", NodeType::TransformComponent,
						addNodePos, { 200.0f, 60.0f}, { 0.6f, 0.3f, 0.3f, 1.0f}));
			}

			if (ImGui::MenuItem("Vec3"))
			{
				std::shared_ptr<Vec3Node> node = m_EntityGraph->AddNode<Vec3Node>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Vec3", NodeType::Vec3,
						addNodePos, { 200.0f, 60.0f}, { 0.3f, 0.6f, 0.3f, 1.0f}));
			}

			ImGui::EndPopup();
		}
	}
}