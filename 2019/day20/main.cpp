#include "common/types.h"

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/Maze.h"
#include "utils/graph/GraphSearchDijkstra.h"
#include "utils/graph/GraphSearchBfs.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"


static bool isBigLetter(char c) {
	return c >= 'A' && c <= 'Z';
}


struct Callbacks : public Maze::Callbacks {
	std::vector<std::string> &map;
	bool solvePortals;
	int  level;

	Callbacks(std::vector<std::string> &map, bool solvePortals = true, int level = 0) : map(map) {
		this->solvePortals = solvePortals;
		this->level        = level;
	}

	virtual ~Callbacks() {
	}

	std::string getPortal(const Point<int> &p) const {
		std::string ret;

		if (isBigLetter(map[p.y()][p.x() + 1])) {
			ret.push_back(map[p.y()][p.x() + 1]);
			ret.push_back(map[p.y()][p.x() + 2]);

		} else if (isBigLetter(map[p.y()][p.x() - 1])) {
			ret.push_back(map[p.y()][p.x() - 2]);
			ret.push_back(map[p.y()][p.x() - 1]);

		} else if (isBigLetter(map[p.y() + 1][p.x()])) {
			ret.push_back(map[p.y() + 1][p.x()]);
			ret.push_back(map[p.y() + 2][p.x()]);

		} else if (isBigLetter(map[p.y() - 1][p.x()])) {
			ret.push_back(map[p.y() - 2][p.x()]);
			ret.push_back(map[p.y() - 1][p.x()]);
		}

		return ret;
	}

	bool isInner(const Point<int> &p) const {
		return ! isOuter(p);
	}

	bool isOuter(const Point<int> &p) const {
		if (
			(p.x() == 2 || p.x() == map[0].length() - 3) ||
			(p.y() == 2 || p.y() == map.size() - 3)
		) {
			return true;
		}

		return false;
	}

	Maze::NodeType getNodeType(const Point<int> &p, char c) const {
		Maze::NodeType ret;

		switch (c) {
			case '#':
				ret = Maze::NodeType::WALL;
				break;

			case '.':
				{
					std::string val = getPortal(p);

					if (val.empty()) {
						ret = Maze::NodeType::PASSAGE;

					} else if (val.compare("AA") == 0) {
						ret = this->level > 0 ? Maze::NodeType::WALL : Maze::NodeType::ENTRANCE;

					} else if (val.compare("ZZ") == 0) {
						ret = this->level > 0 ? Maze::NodeType::WALL : Maze::NodeType::EXIT;

					} else {
						if (! this->solvePortals && (this->level == 0) && isOuter(p)) {
							ret = Maze::NodeType::WALL;

						} else {
							ret = Maze::NodeType::PORTAL;
						}
					}
				}
				break;

			default:
				ret = Maze::NodeType::EMPTY;
		}

		return ret;
	}

	std::string getId(const Point<int> &pos, char c, Maze::NodeType type) const {
		std::string ret = getPortal(pos);

		if (! this->solvePortals) {
			if (
				(pos.x() == 2 || pos.x() == map[0].length() - 3) ||
				(pos.y() == 2 || pos.y() == map.size() - 3)
			) {
				ret = "outer-" + ret;
			} else {
				ret = "inner-" + ret;
			}
		}

		return ret;
	}
};


int main(int argc, char *argv[]) {
	auto inFile = File::readAllLines(argv[1]);

	{
		Maze maze;

		maze.parse(inFile, Callbacks(inFile));
	//	maze.draw();

		graph::Graph graph;
		maze.fillGraph(graph);

		graph::GraphSearchDijkstra dijkstra(&graph);
		std::vector<graph::Node *> route;

		dijkstra.search(maze.getEntrance()->id(), maze.getExit()->id(), route);

		int cost = 0;
		for (int i = 1; i < route.size(); i++) {
			cost += graph.getEdge(route[i - 1]->id(), route[i]->id())->cost();
		}

		PRINTF(("PART_A: %d", cost));
	}

	{
		std::vector<Maze::GraphNode *> nodes;
		std::vector<Maze::GraphEdge *> edges;

		std::map<int, graph::Node *> graphNodes;
		std::vector<graph::Edge *>   graphEdges;

		int entranceId;
		int exitId;

		{
			Maze maze;

			std::map<int, int> innerToOuterMap;
			std::map<int, int> outerToInnerMap;

			maze.parse(inFile, Callbacks(inFile, false, 0));

			// Add level-0
			{
				for (auto n : maze.getNodes()) {
					graphNodes[n->id()] = n;
				}

				for (auto e : maze.getEdges()) {
					graphEdges.push_back(e);
				}
			}

			entranceId = maze.getEntrance()->id();
			exitId     = maze.getExit()->id();

			// Generate graph for deeper layers - they do not have entrance and exit
			maze.parse(inFile, Callbacks(inFile, false, 1));

			nodes = maze.getNodes();
			edges = maze.getEdges();

			// Find inner/outer nodes
			{
				std::map<std::string, std::pair<int, int>> tmp;

				for (auto n : nodes) {
					std::string id = maze.getId(n, Maze::NodeType::PORTAL);

					char subId[10] = { 0 };

					if (id.find("inner") != std::string::npos) {
						sscanf(id.c_str(), "inner-%s", subId);

						tmp[subId].first = n->id();

					} else if (id.find("outer") != std::string::npos) {
						sscanf(id.c_str(), "outer-%s", subId);

						tmp[subId].second = n->id();
					}
				}

				for (auto &p : tmp) {
					innerToOuterMap[p.second.first]  = p.second.second;
					outerToInnerMap[p.second.second] = p.second.first;
				}
			}

			int levelStep   = 1000000;
			int levelOffset = levelStep;

			for (int level = 1; level < 40; level++) {
				// Add level nodes
				for (auto n : nodes) {
					int newId = n->id() + levelOffset;

					graphNodes[newId] = new graph::Node(newId);
				}

				// Add level edges
				for (auto e : edges) {
					int srcId = e->from()->id() + levelOffset;
					int dstId = e->to()->id()   + levelOffset;

					graphEdges.push_back(new graph::Edge(graphNodes[srcId], graphNodes[dstId], e->cost()));
				}

				// connect previous level with current one
				for (auto pair : innerToOuterMap) {
					int srcId = pair.first  + levelOffset - levelStep;
					int dstId = pair.second + levelOffset;

					graphEdges.push_back(new graph::Edge(graphNodes[srcId], graphNodes[dstId], 1));
				}

				for (auto pair : outerToInnerMap) {
					int srcId = pair.first  + levelOffset;
					int dstId = pair.second + levelOffset - levelStep;

					graphEdges.push_back(new graph::Edge(graphNodes[srcId], graphNodes[dstId], 1));
				}

				{
					graph::Graph g;

					for (auto n : graphNodes) {
						g.addNode(n.second);
					}

					for (auto e : graphEdges) {
						g.addValidEdge(e);
					}

					std::vector<graph::Node *> route;

					graph::GraphSearchDijkstra dijkstra(&g);
					dijkstra.search(entranceId, exitId, route);

					if (! route.empty()) {
						int cost = 0;
						for (int i = 1; i < route.size(); i++) {
							graph::Node src(route[i - 1]->id() % levelStep);
							graph::Node dst(route[i]->id() % levelStep);

							cost += g.getEdge(route[i - 1]->id(), route[i]->id())->cost();
						}
						PRINTF(("PART_B: %d", cost));

						break;
					}
				}

				levelOffset += levelStep;
			}
		}
	}
}
