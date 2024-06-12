#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class MultiplyNode : public Node {
	public:
		MultiplyNode()
				: Node(NodeType::Multiply)
		{
			m_InputPorts.resize(2);
			m_OutputPorts.resize(1);

			// Input 1
			m_InputPorts[0] = { nullptr, 1, m_NodeID, PortDataType::Int, "Value 1" };
			// Input 2
			m_InputPorts[1] = { nullptr, 1, m_NodeID, PortDataType::Int, "Value 2" };

			// Output
			m_OutputPorts[0] = { 1, m_NodeID, PortDataType::Int, "Out" };
		}

		virtual ~MultiplyNode() = default;

		void Evaluate() override {
			m_OutputPorts[0].Value = std::get<int>(m_InputPorts[0].GetValue()) * std::get<int>(m_InputPorts[1].GetValue());
		}
	};
}