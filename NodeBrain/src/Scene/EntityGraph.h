#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class EntityGraph
	{
	public:
		EntityGraph() = default;
		~EntityGraph() = default;

		template<typename T, typename... Args>
		std::shared_ptr<T> AddNode(Args&&... args)
		{
			std::shared_ptr<T> node = std::make_shared<T>(std::forward<Args>(args)...);
			m_Nodes[node->GetNodeID()] = node;
			return node;
		}

		// Returns false if cycle is detected.
		bool AddLink(OutputPort& outputPort, InputPort& inputPort);
		void RemoveLink(OutputPort& outputPort, InputPort& inputPort);

		// Run whenever data is updated.
		// Each node only references the input node's data to prevent iterating through the whole graph to get a value.
		// If ran in a game loop, this should be called once at the beginning of each frame.
		void Evaluate();

	private:
		// Returns false if cycle is detected.
		bool TopologicalSort();

	private:
		std::unordered_map<NodeID, std::shared_ptr<Node>> m_Nodes;
		std::unordered_map<NodeID, std::vector<NodeID>> m_AdjList;
		std::vector<NodeID> m_TopSortedNodes;
	};
}