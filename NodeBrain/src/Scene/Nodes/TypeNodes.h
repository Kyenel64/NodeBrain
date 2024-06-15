#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class IntNode : public Node
	{
	public:
		IntNode() : Node(NodeType::Int)
		{
			m_OutputPorts.push_back(OutputPort(*this, PortDataType::Int, 0));
		}
		virtual ~IntNode() = default;

		void Evaluate() override {}
	};



	class FloatNode : public Node
	{
	public:
		FloatNode() : Node(NodeType::Float)
		{
			m_OutputPorts.push_back(OutputPort(*this, PortDataType::Float, 0.0f));
		}
		virtual ~FloatNode() = default;

		void Evaluate() override {}
	};



	class BoolNode : public Node
	{
	public:
		BoolNode() : Node(NodeType::Bool)
		{
			m_OutputPorts.push_back(OutputPort(*this, PortDataType::Bool, true));
		}
		virtual ~BoolNode() = default;

		void Evaluate() override {}
	};



	class StringNode : public Node
	{
	public:
		StringNode() : Node(NodeType::String)
		{
			m_OutputPorts.push_back(OutputPort(*this, PortDataType::String, std::string()));
		}
		virtual ~StringNode() = default;

		void Evaluate() override {}
	};



	class Vec3Node : public Node
	{
	public:
		Vec3Node() : Node(NodeType::Vec3)
		{
			m_InputPorts.push_back(InputPort(*this, PortDataType::Float, 0.0f));
			m_InputPorts.push_back(InputPort(*this, PortDataType::Float, 0.0f));
			m_InputPorts.push_back(InputPort(*this, PortDataType::Float, 0.0f));

			m_OutputPorts.push_back(OutputPort(*this, PortDataType::Vec3, glm::vec3(0.0f)));
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
		Vec4Node() : Node(NodeType::Vec4)
		{
			m_InputPorts.push_back(InputPort(*this, PortDataType::Float, 0.0f));
			m_InputPorts.push_back(InputPort(*this, PortDataType::Float, 0.0f));
			m_InputPorts.push_back(InputPort(*this, PortDataType::Float, 0.0f));
			m_InputPorts.push_back(InputPort(*this, PortDataType::Float, 0.0f));

			m_OutputPorts.push_back(OutputPort(*this, PortDataType::Vec4, glm::vec4(0.0f)));
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
		ColorNode() : Node(NodeType::Color)
		{
			m_OutputPorts.push_back(OutputPort(*this, PortDataType::Color, glm::vec4(1.0f)));
		}

		virtual ~ColorNode() = default;

		void Evaluate() override {}
	};
}