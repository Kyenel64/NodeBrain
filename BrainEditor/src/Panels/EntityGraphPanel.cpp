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
			NodeUI nodeUI;
			for (size_t i = 0; i < node->InputCount(); i++)
				nodeUI.InputPorts.push_back({ node->GetInputPort(i)});
			for (size_t i = 0; i < node->OutputCount(); i++)
				nodeUI.OutputPorts.push_back({ node->GetOutputPort(i)});
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
		m_HoveringNode = false;
		for (auto& nodeUI : m_NodeUIs[m_SelectedEntity])
		{
			if (nodeUI.Type == NodeType::Float)
			{
				DrawNodeUI(nodeUI, [&]()
				{
					ImGui::SetNextItemWidth(nodeUI.Size.x / 2);
					std::shared_ptr<FloatNode> floatNode = std::dynamic_pointer_cast<FloatNode>(nodeUI.OwnedNode);
					ImGui::DragFloat("##Test", &floatNode->GetOutputPort(0).Value.FloatValue);
				});
			}
			else
			{
				DrawNodeUI(nodeUI);
			}
		}

		// Process this after drawing node
		if (m_AddingLink && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			m_AddingLink = false;
			m_SelectedOutputPortUI = nullptr;
		}


		// "Add Node" Popup
		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup("Add Node");
		ProcessAddNodePopup();


		ImGui::End();
	}

	void EntityGraphPanel::DrawNodeUI(NodeUI& node, std::function<void()> uiFunction)
	{
		ImGui::PushID(node.OwnedNode->GetNodeID());
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::SetCursorScreenPos({ m_GridOrigin.x + node.Pos.x, m_GridOrigin.y + node.Pos.y });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.1f, 0.1f, 0.1f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_Border, node.NodeColor);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::BeginChild("NodeFrame", node.Size, ImGuiChildFlags_Border, ImGuiWindowFlags_None);

		// --- Node Header ---
		ImGui::Text("%s", node.NodeName.c_str());

		ImVec2 p1 = ImGui::GetCursorScreenPos();
		drawList->AddLine({ p1.x - ImGui::GetStyle().WindowPadding.x, p1.y }, { p1.x + node.Size.x- ImGui::GetStyle().WindowPadding.x, p1.y }, ImGui::GetColorU32(node.NodeColor));


		// --- Ports ---
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f});

		// Input Ports
		int index = 0;
		for (auto& inputPort : node.InputPorts)
		{
			ImVec2 portScreenPos = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + ((ImGui::GetFontSize() / 2) + ImGui::GetStyle().FramePadding.y)};
			inputPort.PortPos = { portScreenPos.x - m_GridOrigin.x, portScreenPos.y - m_GridOrigin.y };

			std::string label = "##input" + std::to_string(index);

			if (!inputPort.OwnedInputPort.LinkedOutputPort) {

				switch (inputPort.OwnedInputPort.DataType) {
					case PortDataType::None:
						break;
					case PortDataType::Float: {
						ImGui::SetNextItemWidth(node.Size.x / 2);
						ImGui::DragFloat(label.c_str(), &inputPort.OwnedInputPort.DefaultValue.FloatValue);
					}
						break;
				}
			}
			else
			{
				ImGui::InvisibleButton(label.c_str(), { node.Size.x / 2, ImGui::GetFontSize() + ( ImGui::GetStyle().FramePadding.y * 2) });
			}

			drawList->AddCircleFilled(portScreenPos, 5.0f, ImGui::GetColorU32(node.NodeColor));

			// TODO: Move this to beginning of frame or to event callbacks.
			// When link connects to this port, create a link.
			if (m_AddingLink && m_SelectedOutputPortUI &&
				(Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				LinkUI link = { *m_SelectedOutputPortUI, inputPort };
				m_EntityGraph->AddLink(m_SelectedOutputPortUI->OwnedOutputPort, inputPort.OwnedInputPort);
				m_LinkUIs[m_SelectedEntity].push_back(link);
				m_AddingLink = false;
				m_SelectedOutputPortUI = nullptr;
			}

			// When input port is clicked, remove link to the output port. TODO: Implement
			if (!m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) &&
				ImGui::IsMouseClicked(ImGuiMouseButton_Left) && inputPort.OwnedInputPort.LinkedOutputPort)
			{

			}

			index++;
		}

		// Output Ports
		index = 0;
		ImGui::SetCursorScreenPos({ m_GridOrigin.x + node.Pos.x + (node.Size.x / 2), p1.y });
		for (auto& outputPort : node.OutputPorts)
		{
			ImVec2 portScreenPos = { ImGui::GetCursorScreenPos().x + (node.Size.x / 2), ImGui::GetCursorScreenPos().y + ((ImGui::GetFontSize() / 2) + ImGui::GetStyle().FramePadding.y)};
			outputPort.PortPos = { portScreenPos.x - m_GridOrigin.x, portScreenPos.y - m_GridOrigin.y };

			ImGui::Button("Output", { node.Size.x / 2, 0 });

			drawList->AddCircleFilled(portScreenPos, 5.0f, ImGui::GetColorU32(node.NodeColor));

			// When port is clicked, begin adding a link curve.
			if (!m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedOutputPortUI = &outputPort;
				m_AddingLink = true;
			}

			index++;
		}

		ImGui::PopStyleVar();


		// --- Custom Styling ---
		if (uiFunction)
			uiFunction();

		ImGui::EndChild();
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);

		ImGui::PopID();
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
						addNodePos, { 120.0f, 100.0f}, { 0.3f, 0.6f, 0.3f, 1.0f}));
			}

			if (ImGui::MenuItem("Float"))
			{
				std::shared_ptr<FloatNode> node = m_EntityGraph->AddNode<FloatNode>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Float", NodeType::Float,
						addNodePos, { 100.0f, 60.0f}, { 0.3f, 0.6f, 0.6f, 1.0f}));
			}

			ImGui::EndPopup();
		}
	}
}