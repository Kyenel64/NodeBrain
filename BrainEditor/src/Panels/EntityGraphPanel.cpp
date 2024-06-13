#include "EntityGraphPanel.h"

#include <glm/gtc/type_ptr.hpp>

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

		static NodeUI PopulateNodeUI(std::shared_ptr<Node> node, const std::string& name, const ImVec2& pos, const ImVec2& size, const ImVec4& color)
		{
			NodeUI nodeUI;
			for (size_t i = 0; i < node->InputCount(); i++)
				nodeUI.InputPorts.push_back({ node->GetInputPort(i)});
			for (size_t i = 0; i < node->OutputCount(); i++)
				nodeUI.OutputPorts.push_back({ node->GetOutputPort(i)});
			nodeUI.OwnedNode = node;
			nodeUI.NodeName = name;
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

		if (m_SelectedEntity)
		{
			drawList->AddCircle(m_GridOrigin, 5.0f, ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f}));

			// Draw a curve to mouse position if in the process of linking ports
			if (m_AddingLink && ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				ImVec2 portScreenPos = { m_GridOrigin.x + m_SelectedOutputPortUI->PortPos.x, m_GridOrigin.y + m_SelectedOutputPortUI->PortPos.y };
				drawList->AddBezierCubic( portScreenPos, { ImGui::GetMousePos().x, portScreenPos.y },
						{ portScreenPos.x, ImGui::GetMousePos().y }, ImGui::GetMousePos(), ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 0.5f}), 1.0f);
			}

			// Draw all connected links
			for (auto& nodeUI : m_NodeUIs[m_SelectedEntity])
			{
				for (auto& inputPortUI : nodeUI.InputPorts)
				{
					if (inputPortUI.LinkedOutputPortUI)
					{
						OutputPortUI& outputPortUI = *inputPortUI.LinkedOutputPortUI;
						ImVec2 outputPos = { m_GridOrigin.x + outputPortUI.PortPos.x, m_GridOrigin.y + outputPortUI.PortPos.y };
						ImVec2 inputPos = { m_GridOrigin.x + inputPortUI.PortPos.x, m_GridOrigin.y + inputPortUI.PortPos.y };
						drawList->AddBezierCubic( outputPos, { inputPos.x, outputPos.y }, { outputPos.x, inputPos.y }, inputPos,
								ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f}), 1.0f);
					}
				}
			}

			// Draw all nodes in entity graph
			m_HoveringNode = false;
			for (auto& nodeUI : m_NodeUIs[m_SelectedEntity])
			{
				if (nodeUI.OwnedNode->GetType() == NodeType::Int)
				{
					DrawNodeUI(nodeUI, [&]()
					{
						ImGui::DragInt("##DragInt", &std::get<int>(nodeUI.OwnedNode->GetOutputPort(0).Value));
					});
				}
				else if (nodeUI.OwnedNode->GetType() == NodeType::Float)
				{
					DrawNodeUI(nodeUI, [&]()
					{
						ImGui::DragFloat("##DragFloat", &std::get<float>(nodeUI.OwnedNode->GetOutputPort(0).Value));
					});
				}
				else if (nodeUI.OwnedNode->GetType() == NodeType::Bool)
				{
					DrawNodeUI(nodeUI, [&]()
					{
						ImGui::Checkbox("##Checkbox", &std::get<bool>(nodeUI.OwnedNode->GetOutputPort(0).Value));
					});
				}
				else if (nodeUI.OwnedNode->GetType() == NodeType::Color)
				{
					DrawNodeUI(nodeUI, [&]()
					{
						ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs;
						ImGui::ColorEdit4("##ColorEdit4", glm::value_ptr(std::get<glm::vec4>(nodeUI.OwnedNode->GetOutputPort(0).Value)), flags);
					});
				}
				else if (nodeUI.OwnedNode->GetType() == NodeType::String)
				{
					DrawNodeUI(nodeUI, [&]()
					{
						ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
						std::string tag = std::get<std::string>(nodeUI.OwnedNode->GetOutputPort(0).Value).c_str();
						char buffer[256];
						memset(buffer, 0, sizeof(buffer));
						strcpy(buffer, tag.c_str());
						if (ImGui::InputText("##InputText", buffer, sizeof(buffer), flags))
							nodeUI.OwnedNode->GetOutputPort(0).Value = buffer;
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
		}

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

			ImGui::Button(inputPort.OwnedInputPort.Name.c_str(), { node.Size.x / 2, 0 });

			drawList->AddCircleFilled(portScreenPos, 5.0f, ImGui::GetColorU32(node.NodeColor));

			// When link connects to this port, create a link.
			if (m_AddingLink && m_SelectedOutputPortUI &&
				(Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				if (m_SelectedOutputPortUI->OwnedOutputPort.DataType == inputPort.OwnedInputPort.DataType)
				{
					m_EntityGraph->AddLink(m_SelectedOutputPortUI->OwnedOutputPort, inputPort.OwnedInputPort);
					inputPort.LinkedOutputPortUI = m_SelectedOutputPortUI;
				}

				m_AddingLink = false;
				m_SelectedOutputPortUI = nullptr;
			}

			// When input port is clicked, remove link to the output port.
			if (!m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) &&
				ImGui::IsMouseClicked(ImGuiMouseButton_Left) && inputPort.OwnedInputPort.LinkedOutputPort)
			{
				m_EntityGraph->RemoveLink(*inputPort.OwnedInputPort.LinkedOutputPort, inputPort.OwnedInputPort);
				m_SelectedOutputPortUI = inputPort.LinkedOutputPortUI;
				inputPort.LinkedOutputPortUI = nullptr;
				m_AddingLink = true;
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

			ImGui::Button(outputPort.OwnedOutputPort.Name.c_str(), { node.Size.x / 2, 0 });

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

			if (ImGui::MenuItem("Tag Component"))
			{
				std::shared_ptr<TagComponentNode> node = m_EntityGraph->AddNode<TagComponentNode>(m_ActiveScene->GetComponent<TagComponent>(m_SelectedEntity));
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Tag Component",
						addNodePos, { 200.0f, 60.0f}, { 0.6f, 0.3f, 0.3f, 1.0f}));
			}

			if (ImGui::MenuItem("Transform Component"))
			{
				std::shared_ptr<TransformComponentNode> node = m_EntityGraph->AddNode<TransformComponentNode>(m_ActiveScene->GetComponent<TransformComponent>(m_SelectedEntity));
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Transform Component",
						addNodePos, { 200.0f, 100.0f}, { 0.6f, 0.3f, 0.3f, 1.0f}));
			}

			if (ImGui::MenuItem("Sprite Component"))
			{
				if (!m_ActiveScene->HasComponent<SpriteComponent>(m_SelectedEntity))
					m_ActiveScene->AddComponent<SpriteComponent>(m_SelectedEntity);
				std::shared_ptr<SpriteComponentNode> node = m_EntityGraph->AddNode<SpriteComponentNode>(m_ActiveScene->GetComponent<SpriteComponent>(m_SelectedEntity));
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Sprite Component",
						addNodePos, { 200.0f, 60.0f}, { 0.6f, 0.6f, 0.3f, 1.0f}));
			}

			if (ImGui::MenuItem("Int"))
			{
				std::shared_ptr<IntNode> node = m_EntityGraph->AddNode<IntNode>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Int",
						addNodePos, { 100.0f, 60.0f}, { 0.3f, 0.6f, 0.6f, 1.0f}));
			}

			if (ImGui::MenuItem("Float"))
			{
				std::shared_ptr<FloatNode> node = m_EntityGraph->AddNode<FloatNode>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Float",
						addNodePos, { 100.0f, 60.0f}, { 0.3f, 0.6f, 0.6f, 1.0f}));
			}

			if (ImGui::MenuItem("Bool"))
			{
				std::shared_ptr<BoolNode> node = m_EntityGraph->AddNode<BoolNode>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Bool",
						addNodePos, { 100.0f, 60.0f}, { 0.3f, 0.6f, 0.6f, 1.0f}));
			}

			if (ImGui::MenuItem("Vec3"))
			{
				std::shared_ptr<Vec3Node> node = m_EntityGraph->AddNode<Vec3Node>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Vec3",
						addNodePos, { 120.0f, 100.0f}, { 0.3f, 0.6f, 0.3f, 1.0f}));
			}

			if (ImGui::MenuItem("Vec4"))
			{
				std::shared_ptr<Vec4Node> node = m_EntityGraph->AddNode<Vec4Node>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Vec4",
						addNodePos, { 120.0f, 100.0f}, { 0.3f, 0.6f, 0.3f, 1.0f}));
			}

			if (ImGui::MenuItem("Color"))
			{
				std::shared_ptr<ColorNode> node = m_EntityGraph->AddNode<ColorNode>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "Color",
						addNodePos, { 120.0f, 100.0f}, { 0.6f, 0.6f, 0.3f, 1.0f}));
			}

			if (ImGui::MenuItem("String"))
			{
				std::shared_ptr<StringNode> node = m_EntityGraph->AddNode<StringNode>();
				m_NodeUIs[m_SelectedEntity].push_back(Utils::PopulateNodeUI(node, "String",
						addNodePos, { 100.0f, 60.0f}, { 0.3f, 0.6f, 0.6f, 1.0f}));
			}

			ImGui::EndPopup();
		}
	}
}