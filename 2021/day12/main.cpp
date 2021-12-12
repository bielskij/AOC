#include <queue>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph/Graph.h"


#define DEBUG_LEVEL 5
#include "common/debug.h"


class CaveNode : public graph::Node {
	public:
		CaveNode(int id, const std::string &name) : Node(id), name(name) {

		}

		const std::string &getName() const {
			return this->name;
		}

		bool isSmall() const {
			return std::all_of(this->name.begin(), this->name.end(), islower);
		}

	private:
		std::string name;
};


static void findAllPaths(graph::Graph &graph, int globalStart, int globalEnd, int &paths, int start, int end, std::map<int, int> &visited, int *smallTwice) {
	CaveNode *startNode = reinterpret_cast<CaveNode *>(graph.getNode(start));
	CaveNode *endNode   = reinterpret_cast<CaveNode *>(graph.getNode(end));

	if (startNode->isSmall()) {
		visited[start]++;
	}

	if (start == end) {
		paths++;

	} else {
		auto &nextEdges = graph.getEdgeList(start);
		for (const auto edge : nextEdges) {
			CaveNode *adj = reinterpret_cast<CaveNode *>(edge->to());

			if (adj->isSmall()) {
				if (visited[adj->id()] == 0) {
					findAllPaths(graph, globalStart, globalEnd, paths, adj->id(), end, visited, smallTwice);

				} else if (smallTwice != nullptr) {
					if ((adj->id() != globalStart) && (adj->id() != globalEnd) && (*smallTwice == -1)) {
						*smallTwice = adj->id();

						findAllPaths(graph, globalStart, globalEnd, paths, adj->id(), end, visited, smallTwice);
					}
				}

			} else {
				findAllPaths(graph, globalStart, globalEnd, paths, adj->id(), end, visited, smallTwice);
			}
		}
	}

	if (startNode->isSmall()) {
		visited[start]--;

		if (smallTwice != nullptr) {
			if (*smallTwice == start) {
				*smallTwice = -1;
			}
		}
	}
}


static int findAllPaths(graph::Graph &graph, int start, int end, bool partA) {
	std::map<int, int> visited;
	std::vector<int> path;

	int paths = 0;
	int smallTwice = -1;

	findAllPaths(graph, start, end, paths, start, end, visited, partA ? nullptr : &smallTwice);

	return paths;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		std::map<std::string, graph::Node *> nodes;
		std::vector<graph::Edge *> edges;

		{
			int nodeId = 0;

			for (const auto &line : lines) {
				auto pair = utils::strTok(line, '-');

				if (pair.size() != 2) {
					PRINTF(("Unable to parse! '%s'", line.c_str()));
					abort();
				}

				if (nodes.find(pair[0]) == nodes.end()) {
					nodes[pair[0]] = new CaveNode(++nodeId, pair[0]);
				}

				if (nodes.find(pair[1]) == nodes.end()) {
					nodes[pair[1]] = new CaveNode(++nodeId, pair[1]);
				}

				edges.push_back(new graph::Edge(nodes[pair[0]], nodes[pair[1]]));
				edges.push_back(new graph::Edge(nodes[pair[1]], nodes[pair[0]]));
			}
		}

		{
			graph::Graph graph;

			for (const auto &node : nodes) {
				graph.addNode(node.second);
			}

			for (const auto edge : edges) {
				graph.addValidEdge(edge);
			}


			int partA = findAllPaths(graph, nodes["start"]->id(), nodes["end"]->id(), true);
			int partB = findAllPaths(graph, nodes["start"]->id(), nodes["end"]->id(), false);

			PRINTF(("PART_A: %d", partA));
			PRINTF(("PART_B: %d", partB));
		}
	}

	return 0;
}
