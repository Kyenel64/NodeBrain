#include "NBpch.h"
#include "EntityGraph.h"

#include <queue>

namespace NodeBrain
{
	void EntityGraph::AddLink(const std::shared_ptr<Node>& outputNode, uint32_t outputPortIndex,
		const std::shared_ptr<Node>& inputNode, uint32_t inputPortIndex)
	{
		NB_ASSERT(outputNode->m_OutputPorts.size() > outputPortIndex,
			"outputPortIndex out of bounds. outputPortIndex must be less than the total number of output ports in"
			"outputNode");
		NB_ASSERT(inputNode->m_InputPorts.size() > inputPortIndex,
			"inputPortIndex out of bounds. inputPortIndex must be less than the total number of input ports in"
			"inputNode");

		OutputPort& outputPort = outputNode->m_OutputPorts[outputPortIndex];
		InputPort& inputPort = inputNode->m_InputPorts[inputPortIndex];

		inputPort.LinkedOutputPort = &outputPort;

		// Add link to adjacency list
		m_AdjList[outputNode->GetNodeID()].push_back(inputNode->GetNodeID());

		TopologicalSort();
	}

	void EntityGraph::TopologicalSort()
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
	}

	void EntityGraph::Evaluate()
	{
		for (auto& nodeID : m_TopSortedNodes)
			m_Nodes[nodeID]->Evaluate();
	}
}
