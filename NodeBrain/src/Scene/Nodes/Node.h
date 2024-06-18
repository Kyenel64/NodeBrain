#pragma once

#include <string>
#include <vector>
#include <variant>

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
		OutputPort(Node& parentNode, PortDataType type, PortData initialValue, std::string debugName = "Out")
			: m_ParentNode(parentNode), m_Type(type), Value(std::move(initialValue)), m_DebugName(std::move(debugName)) {}

		[[nodiscard]] PortDataType GetType() const { return m_Type; }
		[[nodiscard]] Node& GetParentNode() const { return m_ParentNode; }
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }

	public:
		PortData Value;

	private:
		Node& m_ParentNode;
		PortDataType m_Type;
		std::string m_DebugName;

	public:
		friend class EntityGraph;
	};



	class InputPort
	{
	public:
		InputPort(Node& parentNode, PortDataType type, PortData defaultValue, std::string debugName)
			: m_ParentNode(parentNode), m_Type(type), m_DefaultValue(std::move(defaultValue)), m_DebugName(std::move(debugName)) {}

		[[nodiscard]] const PortData& GetValue() const
		{
			if (m_LinkedOutputPort)
				return m_LinkedOutputPort->Value;
			else
				return m_DefaultValue;
		}

		[[nodiscard]] OutputPort* GetLinkedOutputPort() const { return m_LinkedOutputPort; }
		[[nodiscard]] PortDataType GetType() const { return m_Type; }
		[[nodiscard]] Node& GetParentNode() const { return m_ParentNode; }
		[[nodiscard]] const std::string& GetDebugName() const { return m_DebugName; }

	private:
		OutputPort* m_LinkedOutputPort = nullptr;
		Node& m_ParentNode;
		PortData m_DefaultValue;
		PortDataType m_Type;
		std::string m_DebugName;

	public:
		friend class EntityGraph;
	};



	class Node
	{
	public:
		explicit Node(NodeType type) : m_NodeID(hashIndex), m_Type(type) { hashIndex++; };

		virtual ~Node() = default;

		// Runs the logic of the node and sets output values
		virtual void Evaluate() = 0;

		[[nodiscard]] NodeType GetType() const { return m_Type; }

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

		[[nodiscard]] uint32_t InputCount() const { return m_InputPorts.size(); }
		[[nodiscard]] uint32_t OutputCount() const { return m_OutputPorts.size(); }

	protected:
		std::vector<InputPort> m_InputPorts;
		std::vector<OutputPort> m_OutputPorts;
		const NodeID m_NodeID;
		NodeType m_Type;

	public:
		friend class EntityGraph;
	};
}
