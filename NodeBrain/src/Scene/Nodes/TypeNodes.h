#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class IntNode : public Node
	{
	public:
		IntNode() : Node(NodeType::Int)
		{
			m_OutputPorts.emplace_back(*this, PortDataType::Int, 0);
		}
		~IntNode() override = default;

		void Evaluate() override {}
	};



	class FloatNode : public Node
	{
	public:
		FloatNode() : Node(NodeType::Float)
		{
			m_OutputPorts.emplace_back(*this, PortDataType::Float, 0.0f);
		}
		~FloatNode() override = default;

		void Evaluate() override {}
	};



	class BoolNode : public Node
	{
	public:
		BoolNode() : Node(NodeType::Bool)
		{
			m_OutputPorts.emplace_back(*this, PortDataType::Bool, true);
		}
		~BoolNode() override = default;

		void Evaluate() override {}
	};



	class StringNode : public Node
	{
	public:
		StringNode() : Node(NodeType::String)
		{
			m_OutputPorts.emplace_back(*this, PortDataType::String, std::string());
		}
		~StringNode() override = default;

		void Evaluate() override {}
	};



	class Vec3Node : public Node
	{
	public:
		Vec3Node() : Node(NodeType::Vec3)
		{
			m_InputPorts.emplace_back(*this, PortDataType::Float, 0.0f);
			m_InputPorts.emplace_back(*this, PortDataType::Float, 0.0f);
			m_InputPorts.emplace_back(*this, PortDataType::Float, 0.0f);

			m_OutputPorts.emplace_back(*this, PortDataType::Vec3, glm::vec3(0.0f));
		}

		~Vec3Node() override = default;

		void Evaluate() override
		{
			m_OutputPorts[0].Value = glm::vec3(std::get<float>(m_InputPorts[0].GetValue()),
					std::get<float>(m_InputPorts[1].GetValue()),
					std::get<float>(m_InputPorts[2].GetValue()));
		}
	};



	class Vec4Node : public Node
	{
	public:
		Vec4Node() : Node(NodeType::Vec4)
		{
			m_InputPorts.emplace_back(*this, PortDataType::Float, 0.0f);
			m_InputPorts.emplace_back(*this, PortDataType::Float, 0.0f);
			m_InputPorts.emplace_back(*this, PortDataType::Float, 0.0f);
			m_InputPorts.emplace_back(*this, PortDataType::Float, 0.0f);

			m_OutputPorts.emplace_back(*this, PortDataType::Vec4, glm::vec4(0.0f));
		}

		~Vec4Node() override = default;

		void Evaluate() override
		{
			m_OutputPorts[0].Value = glm::vec4(std::get<float>(m_InputPorts[0].GetValue()),
					std::get<float>(m_InputPorts[1].GetValue()),
					std::get<float>(m_InputPorts[2].GetValue()),
					std::get<float>(m_InputPorts[3].GetValue()));
		}
	};



	class ColorNode : public Node
	{
	public:
		ColorNode() : Node(NodeType::Color)
		{
			m_OutputPorts.emplace_back(*this, PortDataType::Color, glm::vec4(1.0f));
		}

		~ColorNode() override = default;

		void Evaluate() override {}
	};
}