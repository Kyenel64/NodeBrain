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
		// Might add more data so struct for now.
	};

	struct InputPort
	{
		const OutputPort* LinkedOutputPort;
	};

	class Node
	{
	public:
		Node() : m_NodeID(hashIndex) { hashIndex++; };
		virtual ~Node() = default;

		// Runs the logic of the node and sets output values
		virtual void Evaluate() = 0;

		NodeID GetNodeID() const { return m_NodeID; }

		const std::vector<InputPort>& GetInputPorts() { return m_InputPorts; }
		const std::vector<OutputPort>& GetOutputPorts() { return m_OutputPorts; }

	protected:
		std::vector<InputPort> m_InputPorts;
		std::vector<OutputPort> m_OutputPorts;

	private:
		NodeID m_NodeID;

	public:
		friend class EntityGraph;
	};

	class TransformComponentNode : public Node
	{
	public:
		TransformComponentNode(TransformComponent& transform)
			: m_TransformComponent(transform)
		{
			m_InputPorts.resize(1);
			m_InputPorts[0] = InputPort();
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
		IntNode(int initialValue)
		{
			m_OutputPorts.resize(1);
			OutputPort outputPort;
			outputPort.Value.IntValue = initialValue;
			m_OutputPorts[0] = outputPort;
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
		Vec3Node(const glm::vec3& initialValue)
		{
			m_OutputPorts.resize(1);
			OutputPort outputPort;
			outputPort.Value.Vec3Value = initialValue;
			m_OutputPorts[0] = outputPort;
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

			m_InputPorts[0] = InputPort();
			m_InputPorts[1] = InputPort();

			m_OutputPorts[0] = OutputPort();
		}

		virtual ~MultiplyNode() = default;

		void Evaluate() override {
			PortData data;
			data.IntValue =
					m_InputPorts[0].LinkedOutputPort->Value.IntValue * m_InputPorts[1].LinkedOutputPort->Value.IntValue;
			m_OutputPorts[0].Value = data;
		}
	};
}
