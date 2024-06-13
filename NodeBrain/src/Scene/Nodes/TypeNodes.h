#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class IntNode : public Node
	{
	public:
		IntNode() : Node(NodeType::Int)
		{
			m_OutputPorts.resize(1);

			m_OutputPorts[0] = { 1, m_NodeID, PortDataType::Int, "Out" };
		}
		virtual ~IntNode() = default;

		void Evaluate() override {}
	};



	class FloatNode : public Node
	{
	public:
		FloatNode() : Node(NodeType::Float)
		{
			m_OutputPorts.resize(1);

			m_OutputPorts[0] = { 1.0f, m_NodeID, PortDataType::Float, "Out" };
		}
		virtual ~FloatNode() = default;

		void Evaluate() override {}
	};



	class BoolNode : public Node
	{
	public:
		BoolNode() : Node(NodeType::Bool)
		{
			m_OutputPorts.resize(1);

			m_OutputPorts[0] = { true, m_NodeID, PortDataType::Bool, "Out"};
		}
		virtual ~BoolNode() = default;

		void Evaluate() override {}
	};



	class StringNode : public Node
	{
	public:
		StringNode()
				: Node(NodeType::String)
		{
			m_OutputPorts.resize(1);

			m_OutputPorts[0] = { std::string(), m_NodeID, PortDataType::String, "Out" };
		}
		virtual ~StringNode() = default;

		void Evaluate() override {}
	};



	class Vec3Node : public Node
	{
	public:
		Vec3Node()
				: Node(NodeType::Vec3)
		{
			m_InputPorts.resize(3);
			m_OutputPorts.resize(1);

			m_InputPorts[0] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "X" };
			m_InputPorts[1] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "Y" };
			m_InputPorts[2] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "Z" };

			m_OutputPorts[0] = { glm::vec3(0.0f), m_NodeID, PortDataType::Vec3, "Out" };
		}

		virtual ~Vec3Node() = default;

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
		Vec4Node()
				: Node(NodeType::Vec4)
		{
			m_InputPorts.resize(4);
			m_OutputPorts.resize(1);

			m_InputPorts[0] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "X" };
			m_InputPorts[1] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "Y" };
			m_InputPorts[2] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "Z" };
			m_InputPorts[3] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "W" };

			m_OutputPorts[0] = { glm::vec4(0.0f), m_NodeID, PortDataType::Vec4, "Out" };
		}

		virtual ~Vec4Node() = default;

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
		ColorNode()
				: Node(NodeType::Color)
		{
			m_OutputPorts.resize(1);

			m_OutputPorts[0] = { glm::vec4(1.0f), m_NodeID, PortDataType::Color, "Out" };
		}

		virtual ~ColorNode() = default;

		void Evaluate() override {}
	};
}