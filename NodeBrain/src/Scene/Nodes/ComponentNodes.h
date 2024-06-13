#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class TagComponentNode : public Node
	{
	public:
		TagComponentNode(TagComponent& tagComp)
				: m_TagComponent(tagComp), Node(NodeType::TagComponent)
		{
			m_InputPorts.resize(1);

			// Input 1
			m_InputPorts[0] = { nullptr, std::string(), m_NodeID, PortDataType::String, "Tag" };
		}

		void Evaluate() override
		{
			m_TagComponent.Tag = std::get<std::string>(m_InputPorts[0].GetValue());
		}

	private:
		TagComponent& m_TagComponent;
	};



	class TransformComponentNode : public Node
	{
	public:
		TransformComponentNode(TransformComponent& transformComp)
				: m_TransformComponent(transformComp), Node(NodeType::TransformComponent)
		{
			m_InputPorts.resize(3);

			// Input 1
			m_InputPorts[0] = { nullptr, glm::vec3(0.0f), m_NodeID, PortDataType::Vec3, "Position" };

			// Input 2
			m_InputPorts[1] = { nullptr, glm::vec3(0.0f), m_NodeID, PortDataType::Vec3, "Rotation" };

			// Input 3
			m_InputPorts[2] = { nullptr, glm::vec3(1.0f), m_NodeID, PortDataType::Vec3, "Scale" };
		}

		void Evaluate() override
		{
			m_TransformComponent.Position = std::get<glm::vec3>(m_InputPorts[0].GetValue());
			m_TransformComponent.SetEulerRotation(std::get<glm::vec3>(m_InputPorts[1].GetValue()));
			m_TransformComponent.Scale = std::get<glm::vec3>(m_InputPorts[2].GetValue());
		}

	private:
		TransformComponent& m_TransformComponent;
	};



	class SpriteComponentNode : public Node
	{
	public:
		SpriteComponentNode(SpriteComponent& spriteComp)
				: m_SpriteComponent(spriteComp), Node(NodeType::SpriteComponent)
		{
			m_InputPorts.resize(1);

			// Input 1
			m_InputPorts[0] = { nullptr, glm::vec4(1.0f), m_NodeID, PortDataType::Color, "Color" };
		}

		void Evaluate() override
		{
			m_SpriteComponent.Color = std::get<glm::vec4>(m_InputPorts[0].GetValue());
		}

	private:
		SpriteComponent& m_SpriteComponent;
	};
}