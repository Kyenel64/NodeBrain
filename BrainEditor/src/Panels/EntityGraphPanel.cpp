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

	void EntityGraphPanel::Draw(const std::shared_ptr<Scene>& activeScene, Entity selectedEntity)
	{
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
			m_EntityGraph = &activeScene->GetEntityGraph(selectedEntity);

			drawList->AddCircle(m_GridOrigin, 5.0f, ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f}));

			// Draw a curve to mouse position if in the process of linking ports
			if (m_AddingLink && ImGui::IsMouseDown(ImGuiMouseButton_Left))
			{
				const OutputPortUI& selectedOutputPortUI = m_OutputPortUIs[m_SelectedEntity].at(m_SelectedOutputPort);
				ImVec2 portScreenPos = { m_GridOrigin.x + selectedOutputPortUI.PortPos.x, m_GridOrigin.y + selectedOutputPortUI.PortPos.y };
				drawList->AddBezierCubic( portScreenPos, { ImGui::GetMousePos().x, portScreenPos.y },
						{ portScreenPos.x, ImGui::GetMousePos().y }, ImGui::GetMousePos(), ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 0.5f}), 1.0f);
			}

			// Draw all connected links
			for (auto& [ node, nodeUI ] : m_NodeUIs[m_SelectedEntity])
			{
				for (size_t i = 0; i < node->InputCount(); i++)
				{
					InputPort& inputPort = node->GetInputPort(i);
					if (inputPort.GetLinkedOutputPort())
					{
						const InputPortUI& inputPortUI = m_InputPortUIs[m_SelectedEntity].at(&inputPort);
						const OutputPortUI& outputPortUI = m_OutputPortUIs[m_SelectedEntity].at(inputPort.GetLinkedOutputPort());
						ImVec2 outputPos = { m_GridOrigin.x + outputPortUI.PortPos.x, m_GridOrigin.y + outputPortUI.PortPos.y };
						ImVec2 inputPos = { m_GridOrigin.x + inputPortUI.PortPos.x, m_GridOrigin.y + inputPortUI.PortPos.y };
						drawList->AddBezierCubic( outputPos, { inputPos.x, outputPos.y }, { outputPos.x, inputPos.y }, inputPos,
								ImGui::GetColorU32({ 1.0f, 1.0f, 1.0f, 1.0f}), 1.0f);
					}
				}
			}

			// Draw all nodes in entity graph
			for (auto& [ node, nodeUI ] : m_NodeUIs[m_SelectedEntity])
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
						std::string tag = std::get<std::string>(node->GetOutputPort(0).Value);
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


			// Add node popup
			if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				ImGui::OpenPopup("Add Node");
			ProcessAddNodePopup();

		}

		ImGui::End();

		// Nodes are deleted at the end to prevent conflicts.
		while (!m_DeletionQueue.empty())
		{
			Node* node = m_DeletionQueue.front();
			if (node->GetType() == NodeType::SpriteComponent)
				m_ActiveScene->RemoveComponent<SpriteComponent>(m_SelectedEntity);
			m_NodeUIs[m_SelectedEntity].erase(node);
			m_EntityGraph->RemoveNode(*node);

			if (node == m_SelectedNode)
				m_SelectedNode = nullptr;

			m_DeletionQueue.pop();
		}
	}

	void EntityGraphPanel::DrawNodeUI(Node& node, const std::function<void()>& uiFunction)
	{
		NodeUI& nodeUI = m_NodeUIs[m_SelectedEntity].at(&node);

		ImGui::PushID(&node);

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::SetNextWindowPos({ m_GridOrigin.x + nodeUI.Pos.x, m_GridOrigin.y + nodeUI.Pos.y });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.1f, 0.1f, 0.1f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_Border, nodeUI.NodeColor);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::BeginChild("NodeFrame", nodeUI.Size, ImGuiChildFlags_Border, ImGuiWindowFlags_None);

		// Input
		if (ImGui::IsWindowFocused())
			m_SelectedNode = &node;

		if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			nodeUI.Pos.x += io.MouseDelta.x;
			nodeUI.Pos.y += io.MouseDelta.y;
		}

		if (ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup("Node Menu", ImGuiPopupFlags_AnyPopup);
		ProcessNodeMenuPopup();


		// --- Node Header ---
		ImGui::Text("%s, %llu", nodeUI.NodeName.c_str(), (uint64_t)node.m_NodeID); // Node id is debug
		ImVec2 p1 = ImGui::GetCursorScreenPos();
		drawList->AddLine({ p1.x - ImGui::GetStyle().WindowPadding.x, p1.y }, { p1.x + nodeUI.Size.x - ImGui::GetStyle().WindowPadding.x, p1.y },
				ImGui::GetColorU32(nodeUI.NodeColor));


		// --- Ports ---
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { ImGui::GetStyle().ItemSpacing.x, 0.0f});

		// Input Ports
		for (size_t i = 0; i < node.InputCount(); i++)
		{
			InputPort& inputPort = node.GetInputPort(i);
			InputPortUI& inputPortUI = m_InputPortUIs[m_SelectedEntity].at(&inputPort);
			ImVec2 portScreenPos = { ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + ((ImGui::GetFontSize() / 2) + ImGui::GetStyle().FramePadding.y)};
			inputPortUI.PortPos = { portScreenPos.x - m_GridOrigin.x, portScreenPos.y - m_GridOrigin.y };

			std::string label = "##input" + std::to_string(i);

			ImGui::Button(inputPortUI.Name.c_str(), { nodeUI.Size.x / 2, 0 });

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
				m_AddingLink = true;
				m_SelectedOutputPort = inputPort.GetLinkedOutputPort();
				m_EntityGraph->RemoveLink(*inputPort.GetLinkedOutputPort(), inputPort);
			}
		}

		// Output Ports
		ImGui::SetCursorScreenPos({ m_GridOrigin.x + nodeUI.Pos.x + (nodeUI.Size.x / 2), p1.y });
		for (size_t i = 0; i < node.OutputCount(); i++)
		{
			OutputPort& outputPort = node.GetOutputPort(i);
			OutputPortUI& outputPortUI = m_OutputPortUIs[m_SelectedEntity].at(&outputPort);
			ImVec2 portScreenPos = { ImGui::GetCursorScreenPos().x + (nodeUI.Size.x / 2), ImGui::GetCursorScreenPos().y + ((ImGui::GetFontSize() / 2) + ImGui::GetStyle().FramePadding.y)};
			outputPortUI.PortPos = { portScreenPos.x - m_GridOrigin.x, portScreenPos.y - m_GridOrigin.y };

			ImGui::Button(outputPortUI.Name.c_str(), { nodeUI.Size.x / 2, 0 });

			drawList->AddCircleFilled(portScreenPos, 5.0f, ImGui::GetColorU32(nodeUI.NodeColor));

			// When port is clicked, begin adding a link curve.
			if (!m_AddingLink && (Utils::Distance(portScreenPos, ImGui::GetMousePos()) <= 15.0f) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				m_SelectedOutputPort = &outputPort;
				m_AddingLink = true;
			}
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

	void EntityGraphPanel::ProcessNodeMenuPopup()
	{
		if (ImGui::BeginPopup("Node Menu"))
		{
			if (ImGui::MenuItem("Delete"))
				m_DeletionQueue.push(m_SelectedNode);
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
				auto& node = m_EntityGraph->AddNode<TagComponentNode>(m_ActiveScene->GetComponent<TagComponent>(m_SelectedEntity));
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Tag Component", { 0.6f, 0.3f, 0.3f, 1.0f }, { 200.0f, 60.0f }, addNodePos));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(0), InputPortUI("Tag"));
			}

			if (ImGui::MenuItem("Transform Component"))
			{
				auto& node = m_EntityGraph->AddNode<TransformComponentNode>(m_ActiveScene->GetComponent<TransformComponent>(m_SelectedEntity));
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Transform Component", { 0.6f, 0.3f, 0.3f, 1.0f }, { 200.0f, 120.0f }, addNodePos));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(0), InputPortUI("Position"));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(1), InputPortUI("Rotation"));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(2), InputPortUI("Scale"));
			}

			if (ImGui::MenuItem("Sprite Component"))
			{
				if (!m_ActiveScene->HasComponent<SpriteComponent>(m_SelectedEntity))
					m_ActiveScene->AddComponent<SpriteComponent>(m_SelectedEntity);

				auto& node = m_EntityGraph->AddNode<SpriteComponentNode>(m_ActiveScene->GetComponent<SpriteComponent>(m_SelectedEntity));
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Sprite Component", { 0.6f, 0.3f, 0.3f, 1.0f }, { 200.0f, 60.0f }, addNodePos));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(0), InputPortUI("Color"));
			}

			// --- Type Nodes ---
			if (ImGui::MenuItem("Int"))
			{
				auto& node = m_EntityGraph->AddNode<IntNode>();
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Int", { 0.6f, 0.3f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos));
				m_OutputPortUIs[m_SelectedEntity].emplace(&node.GetOutputPort(0), OutputPortUI("Out"));
			}

			if (ImGui::MenuItem("Float"))
			{
				auto& node = m_EntityGraph->AddNode<FloatNode>();
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Float", { 0.6f, 0.3f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos));
				m_OutputPortUIs[m_SelectedEntity].emplace(&node.GetOutputPort(0), OutputPortUI("Out"));
			}

			if (ImGui::MenuItem("Bool"))
			{
				auto& node = m_EntityGraph->AddNode<BoolNode>();
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Bool", { 0.6f, 0.3f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos));
				m_OutputPortUIs[m_SelectedEntity].emplace(&node.GetOutputPort(0), OutputPortUI("Out"));
			}

			if (ImGui::MenuItem("Vec3"))
			{
				auto& node = m_EntityGraph->AddNode<Vec3Node>();
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Vector 3", { 0.6f, 0.3f, 0.3f, 1.0f }, { 100.0f, 80.0f }, addNodePos));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(0), InputPortUI("X"));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(1), InputPortUI("Y"));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(2), InputPortUI("Z"));
				m_OutputPortUIs[m_SelectedEntity].emplace(&node.GetOutputPort(0), OutputPortUI("Out"));
			}

			if (ImGui::MenuItem("Vec4"))
			{
				auto& node = m_EntityGraph->AddNode<Vec4Node>();
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Vector 4", { 0.6f, 0.3f, 0.3f, 1.0f }, { 100.0f, 100.0f }, addNodePos));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(0), InputPortUI("X"));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(1), InputPortUI("Y"));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(2), InputPortUI("Z"));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(3), InputPortUI("W"));
				m_OutputPortUIs[m_SelectedEntity].emplace(&node.GetOutputPort(0), OutputPortUI("Out"));
			}

			if (ImGui::MenuItem("Color"))
			{
				auto& node = m_EntityGraph->AddNode<ColorNode>();
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Color", { 0.6f, 0.3f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos));
				m_OutputPortUIs[m_SelectedEntity].emplace(&node.GetOutputPort(0), OutputPortUI("Out"));
			}

			if (ImGui::MenuItem("String"))
			{
				auto& node = m_EntityGraph->AddNode<StringNode>();
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("String", { 0.6f, 0.3f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos));
				m_OutputPortUIs[m_SelectedEntity].emplace(&node.GetOutputPort(0), OutputPortUI("Out"));
			}

			// --- Math Nodes ---
			if (ImGui::MenuItem("Multiply"))
			{
				auto& node = m_EntityGraph->AddNode<MultiplyNode>();
				m_NodeUIs[m_SelectedEntity].emplace(&node, NodeUI("Multiply", { 0.6f, 0.3f, 0.3f, 1.0f }, { 100.0f, 60.0f }, addNodePos));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(0), InputPortUI("In 1"));
				m_InputPortUIs[m_SelectedEntity].emplace(&node.GetInputPort(1), InputPortUI("In 2"));
				m_OutputPortUIs[m_SelectedEntity].emplace(&node.GetOutputPort(0), OutputPortUI("Sum"));
			}

			ImGui::EndPopup();
		}
	}
}