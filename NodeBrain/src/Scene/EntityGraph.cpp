#include "NBpch.h"
#include "EntityGraph.h"

namespace NodeBrain
{
	void EntityGraph::RemoveNode(Node& removingNode)
	{
		NB_PROFILE_FN();

		NodeID removingNodeID = removingNode.m_NodeID;

		// Remove links between this node's output ports and the linked input ports
		for (auto& [ nodeID, adjNodeID] : m_AdjList)
		{
			if (nodeID == removingNodeID)
			{
				for (auto& inputPort : m_Nodes[adjNodeID]->m_InputPorts)
				{
					if (inputPort.m_LinkedOutputPort->m_ParentNode.m_NodeID == removingNodeID)
						inputPort.m_LinkedOutputPort = nullptr;
				}
			}
		}

		// Remove links between this node's input ports and the linked output ports
		for (auto& inputPort : m_Nodes[removingNode.m_NodeID]->m_InputPorts)
			inputPort.m_LinkedOutputPort = nullptr;

		// Remove all node references from adjacency list.
		for (auto it = m_AdjList.begin(); it != m_AdjList.end(); it++)
		{
			if (it->first == removingNodeID || it->second == removingNodeID)
				m_AdjList.erase(it);
		}

		// Destroy node instance
		m_Nodes.erase(removingNode.m_NodeID);

		TopologicalSort();
	}

	bool EntityGraph::AddLink(OutputPort& outputPort, InputPort& inputPort)
	{
		NB_PROFILE_FN();

		inputPort.m_LinkedOutputPort = &outputPort;
		m_AdjList.emplace_back(outputPort.m_ParentNode.m_NodeID, inputPort.m_ParentNode.m_NodeID);

		// If added link creates a cycle, undo the link and sort.
		if (!TopologicalSort())
		{
			RemoveLink(outputPort, inputPort);
			TopologicalSort();
			NB_ERROR("Failed to create link. Cycle detected.");
			return false;
		}

		return true;
	}

	void EntityGraph::RemoveLink(OutputPort &outputPort, InputPort &inputPort)
	{
		NB_PROFILE_FN();

		inputPort.m_LinkedOutputPort = nullptr;

		for (auto it = m_AdjList.begin(); it != m_AdjList.end(); it++)
		{
			if (it->first == outputPort.m_ParentNode.m_NodeID && it->second == inputPort.m_ParentNode.m_NodeID)
			{
				m_AdjList.erase(it);
				break;
			}
		}

		TopologicalSort();
	}

	bool EntityGraph::TopologicalSort()
	{
		NB_PROFILE_FN();

		m_TopSortedNodes.clear();

		// inDegree == number of connected inputs
		std::unordered_map<NodeID, uint32_t> inDegrees;
		std::queue<NodeID> nodesWithNoInput;

		// Set initial inDegrees to 0 for all nodes.
		for (auto& [ id, node ] : m_Nodes)
			inDegrees[id] = 0;

		// Calculate number of indegrees for each node.
		for (auto& link : m_AdjList)
			inDegrees[link.second]++;

		// Push nodes with an indegree of 0 to queue
		for (auto& [ nodeID, inDegree] : inDegrees)
			if (inDegree == 0)
				nodesWithNoInput.push(nodeID);

		// Retrieve a node with no indegree and push to sorted array.
		// Decrement indegree of adjacent nodes by 1. If the adjacent node has no indegree, add to no indegree queue.
		// Repeat until all nodes have been processed.
		while (!nodesWithNoInput.empty())
		{
			NodeID curNodeID = nodesWithNoInput.front();
			m_TopSortedNodes.push_back(m_Nodes[curNodeID].get());

			for (auto& link : m_AdjList)
			{
				if (link.first == curNodeID)
				{
					inDegrees[link.second]--;
					if (inDegrees[link.second] == 0)
						nodesWithNoInput.push(link.second);
				}
			}
			nodesWithNoInput.pop();
		}

		// Sorted vector must contain all the nodes or there is a cycle.
		if (m_TopSortedNodes.size() != m_Nodes.size())
			return false;

		// Debug
		std::string topSortStr = {};
		for (auto& node : m_TopSortedNodes)
			topSortStr.append(std::to_string(node->m_NodeID) + ", ");
		NB_INFO(topSortStr);

		return true;
	}

	void EntityGraph::Evaluate()
	{
		NB_PROFILE_FN();

		for (auto& node : m_TopSortedNodes)
			node->Evaluate();
	}
}
