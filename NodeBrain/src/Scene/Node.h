#pragma once

#include <glm/glm.hpp>

#include "Scene/Component.h"

namespace NodeBrain
{
	static uint32_t hashIndex = 0; // TODO

	using NodeID = uint32_t;

	enum class NodeType
	{
		None = 0,
		TagComponent,
		TransformComponent,
		Vec3,
		Float,
		Int,
		String,
		Multiply
	};

	enum class PortDataType { None = 0, Int, Float, Vec3, String };

	using PortData = std::variant<int, float, glm::vec3, std::string>;

	struct OutputPort
	{
		PortData Value;
		NodeID ParentNodeID;
		PortDataType DataType;
		std::string Name;
	};

	struct InputPort
	{
		OutputPort* LinkedOutputPort = nullptr;
		PortData DefaultValue;
		NodeID ParentNodeID;
		PortDataType DataType;
		std::string Name;

		const PortData& GetValue() const
		{
			if (LinkedOutputPort)
				return LinkedOutputPort->Value;
			else
				return DefaultValue;
		}
	};

	class Node
	{
	public:
		Node(NodeType type) : m_NodeID(hashIndex), m_Type(type) { hashIndex++; };

		virtual ~Node() = default;

		// Runs the logic of the node and sets output values
		virtual void Evaluate() = 0;

		NodeID GetNodeID() const { return m_NodeID; }
		NodeType GetType() const { return m_Type; }

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
		NodeType m_Type;
	};



	class TransformComponentNode : public Node
	{
	public:
		TransformComponentNode(TransformComponent& transform)
			: m_TransformComponent(transform), Node(NodeType::TransformComponent)
		{
			m_InputPorts.resize(1);

			// Input 1
			m_InputPorts[0] = { nullptr, glm::vec3(0.0f), m_NodeID, PortDataType::Vec3, "Position" };
		}

		void Evaluate() override
		{
			m_TransformComponent.Position = std::get<glm::vec3>(m_InputPorts[0].GetValue());
		}

	private:
		TransformComponent& m_TransformComponent;
	};



	class TagComponentNode : public Node
	{
	public:
		TagComponentNode(TagComponent& tagComponent)
			: m_TagComponent(tagComponent), Node(NodeType::TagComponent)
		{
			m_InputPorts.resize(1);

			// Input 1
			m_InputPorts[0] = { nullptr, std::string(), m_NodeID, PortDataType::String, "Tag" };
		}

		void Evaluate() override
		{
			m_TagComponent.Tag = std::get<std::string>(m_InputPorts[0].GetValue());
		}

	private:
		TagComponent& m_TagComponent;
	};



	class IntNode : public Node
	{
	public:
		IntNode(int initialValue = 1)
			: Node(NodeType::Int)
		{
			m_OutputPorts.resize(1);

			// Output
			m_OutputPorts[0] = { initialValue, m_NodeID, PortDataType::Int, "Out" };
		}
		virtual ~IntNode() = default;

		void SetValue(int val)
		{
			m_OutputPorts[0].Value = val;
		}

		void Evaluate() override {}
	};



	class StringNode : public Node
	{
	public:
		StringNode()
			: Node(NodeType::String)
		{
			m_OutputPorts.resize(1);

			// Output
			m_OutputPorts[0] = { std::string(), m_NodeID, PortDataType::String, "Out" };
		}
		virtual ~StringNode() = default;

		void Evaluate() override {}
	};



	class FloatNode : public Node
	{
	public:
		FloatNode(float initialValue = 1.0f)
			: Node(NodeType::Float)
		{
			m_OutputPorts.resize(1);

			// Output
			m_OutputPorts[0] = { initialValue, m_NodeID, PortDataType::Float, "Out" };
		}
		virtual ~FloatNode() = default;

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

			// Input 1
			m_InputPorts[0] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "X" };
			// Input 2
			m_InputPorts[1] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "Y" };
			// Input 3
			m_InputPorts[2] = { nullptr, 0.0f, m_NodeID, PortDataType::Float, "Z" };

			// Output
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
