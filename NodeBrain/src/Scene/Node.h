#pragma once

#include <glm/glm.hpp>

#include "Scene/Component.h"

namespace NodeBrain
{
	static uint32_t hashIndex = 0; // TODO

	using NodeID = uint32_t;

	enum class PortDataType { None = 0, Int, Float, Vec3 };

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
		PortDataType DataType;
	};

	struct InputPort
	{
		const OutputPort* LinkedOutputPort = nullptr;
		PortData DefaultValue;
		NodeID ParentNodeID;
		PortDataType DataType;

		const PortData& GetValue() const
		{
			return LinkedOutputPort ? LinkedOutputPort->Value : DefaultValue;
		}
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
		const NodeID m_NodeID;
	};



	class TransformComponentNode : public Node
	{
	public:
		TransformComponentNode(TransformComponent& transform)
			: m_TransformComponent(transform)
		{
			m_InputPorts.resize(1);

			// Input 1
			m_InputPorts[0] = { nullptr, { .Vec3Value = glm::vec3(0.0f) }, m_NodeID, PortDataType::Vec3 };
		}

		void Evaluate() override
		{
			m_TransformComponent.Position = m_InputPorts[0].GetValue().Vec3Value;
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
			m_OutputPorts[0] = { { .IntValue = initialValue }, m_NodeID, PortDataType::Int };
		}
		virtual ~IntNode() = default;

		void SetValue(int val)
		{
			m_OutputPorts[0].Value.IntValue = val;
		}

		void Evaluate() override {}
	};



	class FloatNode : public Node
	{
	public:
		FloatNode(float initialValue = 1.0f)
		{
			m_OutputPorts.resize(1);

			// Output
			m_OutputPorts[0] = { { .FloatValue = initialValue }, m_NodeID, PortDataType::Float };
		}
		virtual ~FloatNode() = default;

		void SetValue(float val)
		{
			m_OutputPorts[0].Value.FloatValue = val;
		}

		void Evaluate() override {}
	};



	class Vec3Node : public Node
	{
	public:
		Vec3Node()
		{
			m_InputPorts.resize(3);
			m_OutputPorts.resize(1);

			// Input 1
			m_InputPorts[0] = { nullptr, { .FloatValue = 0.0f }, m_NodeID, PortDataType::Float };
			// Input 2
			m_InputPorts[1] = { nullptr, { .FloatValue = 0.0f }, m_NodeID, PortDataType::Float };
			// Input 3
			m_InputPorts[2] = { nullptr, { .FloatValue = 0.0f }, m_NodeID, PortDataType::Float };

			// Output
			m_OutputPorts[0] = { { .Vec3Value = glm::vec3(0.0f) }, m_NodeID, PortDataType::Vec3 };
		}

		virtual ~Vec3Node() = default;

		void Evaluate() override
		{
			m_OutputPorts[0].Value.Vec3Value.x = m_InputPorts[0].GetValue().FloatValue;
			m_OutputPorts[0].Value.Vec3Value.y = m_InputPorts[1].GetValue().FloatValue;
			m_OutputPorts[0].Value.Vec3Value.z = m_InputPorts[2].GetValue().FloatValue;
		}
	};



	class MultiplyNode : public Node {
	public:
		MultiplyNode() {
			m_InputPorts.resize(2);
			m_OutputPorts.resize(1);

			// Input 1
			m_InputPorts[0] = { nullptr, { .IntValue = 1 }, m_NodeID, PortDataType::Int };
			// Input 2
			m_InputPorts[1] = { nullptr, { .IntValue = 1 }, m_NodeID, PortDataType::Int };

			// Output
			m_OutputPorts[0] = { { .IntValue = 1 }, m_NodeID, PortDataType::Int };
		}

		virtual ~MultiplyNode() = default;

		void Evaluate() override {
			m_OutputPorts[0].Value.IntValue = m_InputPorts[0].GetValue().IntValue * m_InputPorts[1].GetValue().IntValue;
		}
	};
}
