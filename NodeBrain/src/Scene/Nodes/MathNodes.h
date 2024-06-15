#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class MultiplyNode : public Node {
	public:
		MultiplyNode() : Node(NodeType::Multiply)
		{
			m_InputPorts.push_back(InputPort(*this, PortDataType::Int, 1));
			m_InputPorts.push_back(InputPort(*this, PortDataType::Int, 1));

			m_OutputPorts.push_back(OutputPort(*this, PortDataType::Int, 1));
		}

		virtual ~MultiplyNode() = default;

		void Evaluate() override {
			m_OutputPorts[0].Value = std::get<int>(m_InputPorts[0].GetValue()) * std::get<int>(m_InputPorts[1].GetValue());
		}
	};
}