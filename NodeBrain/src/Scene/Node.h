#pragma once

#include <glm/glm.hpp>

#include "Scene/Component.h"

namespace NodeBrain
{
	static uint32_t hashIndex = 0; // TODO

	using NodeID = uint32_t;

	union PortData
	{
		int IntValue;
		float FloatValue;
		glm::vec3 Vec3Value;
	};

	struct OutputPort
	{
		PortData Value;
		NodeID ParentNodeID;
	};

	struct InputPort
	{
		const OutputPort* LinkedOutputPort = nullptr;
		NodeID ParentNodeID;
	};

	class Node
	{
	public:
		Node() : m_NodeID(hashIndex) { hashIndex++; };
		virtual ~Node() = default;

		// Runs the logic of the node and sets output values
		virtual void Evaluate() = 0;

		NodeID GetNodeID() const { return m_NodeID; }

		InputPort& GetInputPort(uint32_t index)
		{
			NB_ASSERT(index < m_InputPorts.size(), "Index must be less than the total input count.");
			return m_InputPorts[index];
		}
		OutputPort& GetOutputPort(uint32_t index)
		{
			NB_ASSERT(index < m_OutputPorts.size(), "Index must be less than the total output count.");
			return m_OutputPorts[index];
		}

		uint32_t InputCount() const { return m_InputPorts.size(); }
		uint32_t OutputCount() const { return m_OutputPorts.size(); }

	protected:
		std::vector<InputPort> m_InputPorts;
		std::vector<OutputPort> m_OutputPorts;
		NodeID m_NodeID;
	};



	class TransformComponentNode : public Node
	{
	public:
		TransformComponentNode(TransformComponent& transform)
			: m_TransformComponent(transform)
		{
			m_InputPorts.resize(1);

			// Input
			InputPort inputPort;
			inputPort.ParentNodeID = m_NodeID;
			m_InputPorts[0] = std::move(inputPort);
		}

		void Evaluate() override
		{
			m_TransformComponent.Position = m_InputPorts[0].LinkedOutputPort->Value.Vec3Value;
		}

	private:
		TransformComponent& m_TransformComponent;
	};



	class IntNode : public Node
	{
	public:
		IntNode(int initialValue = 1)
		{
			m_OutputPorts.resize(1);

			// Output
			OutputPort outputPort;
			outputPort.ParentNodeID = m_NodeID;
			outputPort.Value.IntValue = initialValue;
			m_OutputPorts[0] = std::move(outputPort);
		}
		virtual ~IntNode() = default;

		void SetValue(int val)
		{
			m_OutputPorts[0].Value.IntValue = val;
		}

		void Evaluate() override {}
	};



	class Vec3Node : public Node
	{
	public:
		Vec3Node(const glm::vec3& initialValue = { 1.0f, 1.0f, 1.0f })
		{
			m_OutputPorts.resize(1);

			// Output
			OutputPort outputPort;
			outputPort.ParentNodeID = m_NodeID;
			outputPort.Value.Vec3Value = initialValue;
			m_OutputPorts[0] = std::move(outputPort);
		}
		virtual ~Vec3Node() = default;

		void SetValue(const glm::vec3& val)
		{
			m_OutputPorts[0].Value.Vec3Value = val;
		}

		void Evaluate() override {}
	};



	class MultiplyNode : public Node {
	public:
		MultiplyNode() {
			m_InputPorts.resize(2);
			m_OutputPorts.resize(1);

			// Input 1
			InputPort inputPort1;
			inputPort1.ParentNodeID = m_NodeID;
			m_InputPorts[0] = std::move(inputPort1);

			// Input 2
			InputPort inputPort2;
			inputPort2.ParentNodeID = m_NodeID;
			m_InputPorts[1] = std::move(inputPort2);

			// Output
			OutputPort outputPort;
			outputPort.ParentNodeID = m_NodeID;
			outputPort.Value.IntValue = 0; // TODO: Calculate initial value
			m_OutputPorts[0] = std::move(outputPort);
		}

		virtual ~MultiplyNode() = default;

		void Evaluate() override {
			m_OutputPorts[0].Value.IntValue = m_InputPorts[0].LinkedOutputPort->Value.IntValue * m_InputPorts[1].LinkedOutputPort->Value.IntValue;
		}
	};
}
