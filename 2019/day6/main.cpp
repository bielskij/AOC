#include <iostream>
#include <map>
#include <stack>
#include <queue>
#include <limits.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph/Graph.h"
#include "utils/graph/GraphSearchBfs.h"
#include "utils/graph/GraphSearchDijkstra.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"


class OrbitNode : public graph::Node {
	public:
		OrbitNode(int id, const std::string &name) : Node(id), name(name) {

		}

		const std::string &getName() const {
			return this->name;
		}

	private:
		std::string name;
};


int main(int argc, char *argv[]) {
	graph::Graph graph;

	std::map<std::string, graph::Node *> graphNodes;

	{
		std::vector<std::string> map = File::readAllLines(argv[1]);

		int nodeId = 0;

		for (auto it = map.begin(); it != map.end(); it++) {
			std::vector<std::string> entry = utils::strTok(*it, ')');

			if (graphNodes.find(entry[0]) == graphNodes.end()) {
				graphNodes[entry[0]] = new OrbitNode(++nodeId, entry[0]);;
			}

			if (graphNodes.find(entry[1]) == graphNodes.end()) {
				graphNodes[entry[1]] = new OrbitNode(++nodeId, entry[1]);
			}
		}

		for (auto &node : graphNodes) {
			graph.addNode(node.second);
		}

		for (auto it = map.begin(); it != map.end(); it++) {
			std::vector<std::string> entry = utils::strTok(*it, ')');

			graph::Node *l = graphNodes[entry[0]];
			graph::Node *r = graphNodes[entry[1]];

			graph.addEdge(l->id(), r->id(), 1);
			graph.addEdge(r->id(), l->id(), 1);
		}
	}

	{
		graph::GraphSearchBfs bfs(&graph);

		std::vector<graph::Node *> route;

		int targetId = graphNodes["COM"]->id();

		int sum = 0;
		for (auto &n : graphNodes) {
			if (n.second->id() != targetId) {
				bfs.search(n.second->id(), targetId, route, false);

				sum += route.size() - 1;
			}
		}

		PRINTF(("PART_A: %d", sum));
	}

	{
		graph::GraphSearchDijkstra dijkstra(&graph);

		std::vector<graph::Node *> route;

		dijkstra.search(graphNodes["YOU"]->id(), graphNodes["SAN"]->id(), route);

		PRINTF(("PART_B: %zd", route.size() - 3));
	}
}
