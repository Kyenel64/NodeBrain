#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Core/Log.h"
#include "Scene/Component.h"

namespace NodeBrain
{
	static uint32_t hashIndex = 1; // TODO

	using NodeID = uint32_t;

	enum class NodeType
	{
		None = 0,
		TagComponent,
		TransformComponent,
		SpriteComponent,

		Int,
		Float,
		Bool,
		Vec3,
		Vec4,
		Color,
		String,

		Multiply
	};

	enum class PortDataType { None = 0, Int, Float, Bool, Vec3, Vec4, Color, String };

	using PortData = std::variant<int, float, bool, glm::vec3, glm::vec4, std::string>;

	class Node;

	struct OutputPort
	{
	public:
		OutputPort(Node& parentNode, PortDataType type, PortData initialValue)
			: m_ParentNode(parentNode), m_Type(type), Value(initialValue) {}

		PortDataType GetType() const { return m_Type; }
		Node& GetParentNode() const { return m_ParentNode; }

	public:
		PortData Value;

	private:
		Node& m_ParentNode;
		PortDataType m_Type;

	public:
		friend class EntityGraph;
	};



	class InputPort
	{
	public:
		InputPort(Node& parentNode, PortDataType type, PortData defaultValue)
			: m_ParentNode(parentNode), m_Type(type), m_DefaultValue(defaultValue) {}

		const PortData& GetValue() const
		{
			if (m_LinkedOutputPort)
				return m_LinkedOutputPort->Value;
			else
				return m_DefaultValue;
		}

		OutputPort* GetLinkedOutputPort() const { return m_LinkedOutputPort; }
		PortDataType GetType() const { return m_Type; }
		Node& GetParentNode() const { return m_ParentNode; }

	private:
		OutputPort* m_LinkedOutputPort = nullptr;
		Node& m_ParentNode;
		PortData m_DefaultValue;
		PortDataType m_Type;

	public:
		friend class EntityGraph;
	};



	class Node
	{
	public:
		Node(NodeType type) : m_NodeID(hashIndex), m_Type(type) { hashIndex++; };

		virtual ~Node() = default;

		// Runs the logic of the node and sets output values
		virtual void Evaluate() = 0;

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

	public:
		friend class EntityGraph;
	};
}
