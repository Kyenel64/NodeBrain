#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class TagComponentNode : public Node
	{
	public:
		explicit TagComponentNode(TagComponent& tagComp)
				: m_TagComponent(tagComp), Node(NodeType::TagComponent)
		{
			m_InputPorts.emplace_back(*this, PortDataType::String, std::string());
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
		explicit TransformComponentNode(TransformComponent& transformComp)
				: m_TransformComponent(transformComp), Node(NodeType::TransformComponent)
		{
			m_InputPorts.emplace_back(*this, PortDataType::Vec3, glm::vec3(0.0f));
			m_InputPorts.emplace_back(*this, PortDataType::Vec3, glm::vec3(0.0f));
			m_InputPorts.emplace_back(*this, PortDataType::Vec3, glm::vec3(1.0f));
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
		explicit SpriteComponentNode(SpriteComponent& spriteComp)
				: m_SpriteComponent(spriteComp), Node(NodeType::SpriteComponent)
		{
			m_InputPorts.emplace_back(*this, PortDataType::Color, glm::vec4(1.0f));
		}

		void Evaluate() override
		{
			m_SpriteComponent.Color = std::get<glm::vec4>(m_InputPorts[0].GetValue());
		}

	private:
		SpriteComponent& m_SpriteComponent;
	};
}