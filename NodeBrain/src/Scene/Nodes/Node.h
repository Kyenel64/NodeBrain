#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Core/Log.h"
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
		SpriteComponent,
		Vec3,
		Vec4,
		Color,
		Float,
		Int,
		String,
		Multiply
	};

	enum class PortDataType { None = 0, Int, Float, Vec3, Vec4, Color, String };

	using PortData = std::variant<int, float, glm::vec3, glm::vec4, std::string>;

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
}
