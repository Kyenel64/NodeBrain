#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class MultiplyNode : public Node {
	public:
		MultiplyNode() : Node(NodeType::Multiply)
		{
			m_InputPorts.emplace_back(*this, PortDataType::Int, 1, "In 1");
			m_InputPorts.emplace_back(*this, PortDataType::Int, 1, "In 2");

			m_OutputPorts.emplace_back(*this, PortDataType::Int, 1);
		}

		~MultiplyNode() override = default;

		void Evaluate() override {
			m_OutputPorts[0].Value = std::get<int>(m_InputPorts[0].GetValue()) * std::get<int>(m_InputPorts[1].GetValue());
		}
	};
}