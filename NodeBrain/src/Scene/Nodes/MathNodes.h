#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class MultiplyNode : public Node {
	public:
		MultiplyNode() : Node(NodeType::Multiply)
		{
			NB_PROFILE_FN();

			m_InputPorts.emplace_back(*this, PortDataType::Int, 1);
			m_InputPorts.emplace_back(*this, PortDataType::Int, 1);

			m_OutputPorts.emplace_back(*this, PortDataType::Int, 1);
		}

		~MultiplyNode() override = default;

		void Evaluate() override
		{
			NB_PROFILE_FN();

			m_OutputPorts[0].Value = std::get<int>(m_InputPorts[0].GetValue()) * std::get<int>(m_InputPorts[1].GetValue());
		}
	};
}