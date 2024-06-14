#include "NBpch.h"
#include "EntityGraph.h"

#include <queue>

namespace NodeBrain
{
	bool EntityGraph::AddLink(OutputPort& outputPort, InputPort& inputPort)
	{
		inputPort.LinkedOutputPort = &outputPort;
		m_AdjList[outputPort.ParentNodeID].push_back(inputPort.ParentNodeID);

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

	void EntityGraph::RemoveNode(NodeID node)
	{
		// Remove all input ports within entity graph that are connected to the output port of current node.
		for (auto& adjNodes : m_AdjList[node])
		{
			std::shared_ptr<Node>& n = m_Nodes[adjNodes];
			for (auto& inputPort : n->m_InputPorts)
			{
				if (inputPort.LinkedOutputPort->ParentNodeID == node)
					inputPort.LinkedOutputPort = nullptr;
			}
		}

		// Remove links from all input ports of current node.
		for (auto& inputPort : m_Nodes[node]->m_InputPorts)
			inputPort.LinkedOutputPort = nullptr;

		// Remove node from adjacency list.
		m_AdjList.erase(node);
		for (auto& [ nodeID, adjNodes ] : m_AdjList)
		{
			for (auto& adjNode : adjNodes)
			{
				if (adjNode == node)
					adjNodes.erase(std::find(adjNodes.begin(), adjNodes.end(), node));
			}
		}

		// Destroy node instance
		m_Nodes.erase(node);

		TopologicalSort();
	}

	void EntityGraph::RemoveLink(OutputPort &outputPort, InputPort &inputPort)
	{
		inputPort.LinkedOutputPort = nullptr;
		std::vector<NodeID>& adjNodes = m_AdjList[outputPort.ParentNodeID];
		adjNodes.erase(std::find(adjNodes.begin(), adjNodes.end(), inputPort.ParentNodeID));

		TopologicalSort();
	}

	bool EntityGraph::TopologicalSort()
	{
		m_TopSortedNodes.clear();

		// inDegree == number of connected inputs
		std::unordered_map<NodeID, uint32_t> inDegrees;
		std::queue<NodeID> nodesWithNoInput;

		// Set initial inDegrees to 0 for all nodes.
		for (auto& [ id, node ] : m_Nodes)
			inDegrees[id] = 0;

		// Calculate number of indegrees for each node.
		for (auto& [ nodeID, adjNodeIDs] : m_AdjList)
		{
			for (auto adjNodeID : adjNodeIDs)
				inDegrees[adjNodeID]++;
		}

		// Push nodes with an indegree of 0 to queue
		for (auto& [ nodeID, inDegree] : inDegrees)
			if (inDegree == 0)
				nodesWithNoInput.push(nodeID);

		// Retrieve a node with no indegree and push to sorted array.
		// Decrement indegree of adjacent nodes by 1. If the adjacent node has no indegree, add to no indegree queue.
		// Repeat until all nodes have been processed.
		while (!nodesWithNoInput.empty())
		{
			NodeID curNode = nodesWithNoInput.front();
			m_TopSortedNodes.push_back(curNode);

			for (auto& adjNode : m_AdjList[curNode])
			{
				inDegrees[adjNode]--;
				if (inDegrees[adjNode] == 0)
					nodesWithNoInput.push(adjNode);
			}
			nodesWithNoInput.pop();
		}

		if (m_TopSortedNodes.size() != m_Nodes.size())
			return false;

		// Debug
		std::string topSortStr = {};
		for (auto& nodeID : m_TopSortedNodes)
			topSortStr.append(std::to_string(nodeID) + ", ");
		NB_INFO(topSortStr);

		return true;
	}

	void EntityGraph::Evaluate()
	{
		for (auto& nodeID : m_TopSortedNodes)
			m_Nodes[nodeID]->Evaluate();
	}
}
