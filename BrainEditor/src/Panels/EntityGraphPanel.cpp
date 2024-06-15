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
				OutputPortUI& selectedOutputPortUI = m_OutputPortUIs[m_SelectedOutputPort];
				ImVec2 portScreenPos = { m_GridOrigin.x + selectedOutputPortUI.PortPos.x, m_GridOrigin.y + selectedOutputPortUI.PortPos.y };
				drawList->AddBezierCubic( portScreenPos, { ImGui::GetMousePos().x, portScreenPos.y },
						{ portScreenPos.x, ImGui::GetMousePos().y }, ImGui::GetMousePos(), ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 0.5f}), 1.0f);
			}

			// Draw all connected links
			for (auto& [ node, nodeUI ] : m_NodeUIs)
			{
				for (size_t i = 0; i < node->InputCount(); i++)
				{
					InputPort& inputPort = node->GetInputPort(i);
					if (inputPort.GetLinkedOutputPort())
					{
						InputPortUI& inputPortUI = m_InputPortUIs.at(&inputPort);
						OutputPortUI& outputPortUI = m_OutputPortUIs.at(inputPort.GetLinkedOutputPort());
						ImVec2 outputPos = { m_GridOrigin.x + outputPortUI.PortPos.x, m_GridOrigin.y + outputPortUI.PortPos.y };
						ImVec2 inputPos = { m_GridOrigin.x + inputPortUI.PortPos.x, m_GridOrigin.y + inputPortUI.PortPos.y };
						drawList->AddBezierCubic( outputPos, { inputPos.x, outputPos.y }, { outputPos.x, inputPos.y }, inputPos,
								ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f}), 1.0f);
					}
				}
			}

			// Draw all nodes in entity graph
			for (auto& [ node, nodeUI ] : m_NodeUIs)
			{
				if (node->GetType() == NodeType::Int)
				{
					DrawNodeUI(*node, [&]()
					{
						ImGui::DragInt("##DragInt", &std::get<int>(node->GetOutputPort(0).Value));
					});
				}
				else if (node->GetType() == NodeType::Float)
				{
					DrawNodeUI(*node, [&]()
					{
						ImGui::DragFloat("##DragFloat", &std::get<float>(node->GetOutputPort(0).Value));
					});
				}
				else if (node->GetType() == NodeType::Bool)
				{
					DrawNodeUI(*node, [&]()
					{
						ImGui::Checkbox("##Checkbox", &std::get<bool>(node->GetOutputPort(0).Value));
					});
				}
				else if (node->GetType() == NodeType::Color)
				{
					DrawNodeUI(*node, [&]()
					{
						ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs;
						ImGui::ColorEdit4("##ColorEdit4", glm::value_ptr(std::get<glm::vec4>(node->GetOutputPort(0).Value)), flags);
					});
				}
				else if (node->GetType() == NodeType::String)
				{
					DrawNodeUI(*node, [&]()
					{
						ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
						std::string tag = std::get<std::string>(node->GetOutputPort(0).Value).c_str();
						char buffer[256];
						memset(buffer, 0, sizeof(buffer));
						strcpy(buffer, tag.c_str());
						if (ImGui::InputText("##InputText", buffer, sizeof(buffer), flags))
							node->GetOutputPort(0).Value = buffer;
					});
				}
				else
				{
					DrawNodeUI(*node);
				}
			}

			// Process this after drawing node
			if (m_AddingLink && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				m_AddingLink = false;
				m_SelectedOutputPort = nullptr;
			}


			// "Add Node" Popup
			if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				ImGui::OpenPopup("Add Node");
			ProcessAddNodePopup();

			// Deleting nodes must be handled at the end.
			if (m_SelectedNode && ImGui::IsKeyPressed(ImGuiKey_Backspace))
			{
				if (m_SelectedNode->GetType() == NodeType::SpriteComponent)
					m_ActiveScene->RemoveComponent<SpriteComponent>(m_SelectedEntity);
				m_NodeUIs.erase(m_SelectedNode);
				m_EntityGraph->RemoveNode(*m_SelectedNode);
			}
		}

		ImGui::End();
	}

	void EntityGraphPanel::DrawNodeUI(Node& node, std::function<void()> uiFunction)
	{
		NodeUI& nodeUI = m_NodeUIs[&node];
		ImGui::PushID(&node);
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::SetCursorScreenPos({ m_GridOrigin.x + nodeUI.Pos.x, m_GridOrigin.y + nodeUI.Pos.y });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.1f, 0.1f, 0.1f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_Border, nodeUI.NodeColor);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::BeginChild("NodeFrame", nodeUI.Size, ImGuiChildFlags_Border, ImGuiWindowFlags_None);

		// --- Node Header ---
		ImGui::Text("%s", nodeUI.NodeName.c_str());

		ImVec2 p1 = ImGui::GetCursorScreenPos();
		drawList->AddLine({ p1.x - ImGui::GetStyle().WindowPadding.x, p1.y }, { p1.x + nodeUI.Size.x- ImGui::GetStyle().WindowPadding.x, p1.y }, ImGui::GetColorU32(nodeUI.NodeColor));


		// --- Ports ---
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f});

		// Input Ports
		int index = 0;
		for (size_t i = 0; i < node.InputCount(); i++)
		{
			InputPort& inputPort = node.GetInputPort(i);
			InputPortUI& inputPortUI = m_InputPortUIs[&inputPort];
			ImVec2 portScreenPos = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + ((ImGui::GetFontSize() / 2) + ImGui::GetStyle().FramePadding.y)};
			inputPortUI.PortPos = { portScreenPos.x - m_GridOrigin.x, portScreenPos.y - m_GridOrigin.y };

			std::string label = "##input" + std::to_string(index);

			ImGui::Button("Placeholder", { nodeUI.Size.x / 2, 0 });

			drawList->AddCircleFilled(portScreenPos, 5.0f, ImGui::GetColorU32(nodeUI.NodeColor));

			// When link connects to this port, create a link.
			if (m_AddingLink && m_SelectedOutputPort &&
				(Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				if (m_SelectedOutputPort->GetType() == inputPort.GetType())
					m_EntityGraph->AddLink(*m_SelectedOutputPort, inputPort);

				m_AddingLink = false;
				m_SelectedOutputPort = nullptr;
			}

			// When input port is clicked, remove link to the output port.
			if (!m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) &&
				ImGui::IsMouseClicked(ImGuiMouseButton_Left) && inputPort.GetLinkedOutputPort())
			{
				m_EntityGraph->RemoveLink(*inputPort.GetLinkedOutputPort(), inputPort);
				m_SelectedOutputPort = inputPort.GetLinkedOutputPort();
				m_AddingLink = true;
			}

			index++;
		}

		// Output Ports
		index = 0;
		ImGui::SetCursorScreenPos({ m_GridOrigin.x + nodeUI.Pos.x + (nodeUI.Size.x / 2), p1.y });
		for (size_t i = 0; i < node.OutputCount(); i++)
		{
			OutputPort& outputPort = node.GetOutputPort(i);
			OutputPortUI& outputPortUI = m_OutputPortUIs[&outputPort];
			ImVec2 portScreenPos = { ImGui::GetCursorScreenPos().x + (nodeUI.Size.x / 2), ImGui::GetCursorScreenPos().y + ((ImGui::GetFontSize() / 2) + ImGui::GetStyle().FramePadding.y)};
			outputPortUI.PortPos = { portScreenPos.x - m_GridOrigin.x, portScreenPos.y - m_GridOrigin.y };

			ImGui::Button("Placeholder", { nodeUI.Size.x / 2, 0 });

			drawList->AddCircleFilled(portScreenPos, 5.0f, ImGui::GetColorU32(nodeUI.NodeColor));

			// When port is clicked, begin adding a link curve.
			if (!m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedOutputPort = &outputPort;
				m_AddingLink = true;
			}

			index++;
		}

		ImGui::PopStyleVar();


		// --- Custom Styling ---
		if (uiFunction)
			uiFunction();


		// Input
		if (ImGui::IsWindowFocused())
		{
			m_SelectedNode = &node;

			if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				nodeUI.Pos.x += io.MouseDelta.x;
				nodeUI.Pos.y += io.MouseDelta.y;
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);

		ImGui::PopID();
	}

	void EntityGraphPanel::ProcessNodeMenuPopup()
	{
		if (ImGui::BeginPopup("Node Menu"))
		{
			if (ImGui::MenuItem("Delete"))
			{
				m_EntityGraph->RemoveNode(*m_SelectedNode);
			}
			ImGui::EndPopup();
		}
	}

	void EntityGraphPanel::ProcessAddNodePopup()
	{
		if (ImGui::BeginPopup("Add Node"))
		{
			const ImVec2 addNodePos = { ImGui::GetMousePosOnOpeningCurrentPopup().x - m_GridOrigin.x, ImGui::GetMousePosOnOpeningCurrentPopup().y - m_GridOrigin.y };

			// --- Component Nodes ---
			if (ImGui::MenuItem("Tag Component"))
			{
				TagComponentNode& node = m_EntityGraph->AddNode<TagComponentNode>(m_ActiveScene->GetComponent<TagComponent>(m_SelectedEntity));
				m_NodeUIs[&node] = { "Tag Component", { 0.6f, 0.3f, 0.3f, 1.0f }, { 200.0f, 60.0f }, addNodePos };
			}

			if (ImGui::MenuItem("Transform Component"))
			{
				TransformComponentNode& node = m_EntityGraph->AddNode<TransformComponentNode>(m_ActiveScene->GetComponent<TransformComponent>(m_SelectedEntity));
				m_NodeUIs[&node] = { "Transform Component", { 0.6f, 0.3f, 0.3f, 1.0f }, { 200.0f, 120.0f }, addNodePos };
			}

			if (ImGui::MenuItem("Sprite Component"))
			{
				if (!m_ActiveScene->HasComponent<SpriteComponent>(m_SelectedEntity))
					m_ActiveScene->AddComponent<SpriteComponent>(m_SelectedEntity);
				SpriteComponentNode& node = m_EntityGraph->AddNode<SpriteComponentNode>(m_ActiveScene->GetComponent<SpriteComponent>(m_SelectedEntity));
				m_NodeUIs[&node] = { "Sprite Component", { 0.6f, 0.3f, 0.3f, 1.0f }, { 200.0f, 60.0f }, addNodePos };
			}

			// --- Type Nodes ---
			if (ImGui::MenuItem("Int"))
			{
				IntNode& node = m_EntityGraph->AddNode<IntNode>();
				m_NodeUIs[&node] = { "Int", { 0.3f, 0.6f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos };
			}

			if (ImGui::MenuItem("Float"))
			{
				FloatNode& node = m_EntityGraph->AddNode<FloatNode>();
				m_NodeUIs[&node] = { "Float", { 0.3f, 0.6f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos };
			}

			if (ImGui::MenuItem("Bool"))
			{
				BoolNode& node = m_EntityGraph->AddNode<BoolNode>();
				m_NodeUIs[&node] = { "Bool", { 0.3f, 0.6f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos };
			}

			if (ImGui::MenuItem("Vec3"))
			{
				Vec3Node& node = m_EntityGraph->AddNode<Vec3Node>();
				m_NodeUIs[&node] = { "Vector 3", { 0.3f, 0.6f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos };
			}

			if (ImGui::MenuItem("Vec4"))
			{
				Vec4Node& node = m_EntityGraph->AddNode<Vec4Node>();
				m_NodeUIs[&node] = { "Vector 4", { 0.3f, 0.6f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos };
			}

			if (ImGui::MenuItem("Color"))
			{
				ColorNode& node = m_EntityGraph->AddNode<ColorNode>();
				m_NodeUIs[&node] = { "Color", { 0.3f, 0.6f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos };
			}

			if (ImGui::MenuItem("String"))
			{
				StringNode& node = m_EntityGraph->AddNode<StringNode>();
				m_NodeUIs[&node] = { "String", { 0.3f, 0.6f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos };
			}

			// --- Math Nodes ---
			if (ImGui::MenuItem("Multiply"))
			{
				MultiplyNode& node = m_EntityGraph->AddNode<MultiplyNode>();
				m_NodeUIs[&node] = { "Multiply", { 0.3f, 0.3f, 0.6f, 1.0f }, { 100.0f, 60.0f }, addNodePos };
			}

			ImGui::EndPopup();
		}
	}
}