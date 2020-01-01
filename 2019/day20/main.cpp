#include "common/types.h"

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/Maze.h"
#include "utils/graph/GraphSearchDijkstra.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"


static bool isBigLetter(char c) {
	return c >= 'A' && c <= 'Z';
}


struct Callbacks : public Maze::Callbacks {
	std::vector<std::string> &map;

	Callbacks(std::vector<std::string> &map) : map(map) {
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
						ret = Maze::NodeType::ENTRANCE;

					} else if (val.compare("ZZ") == 0) {
						ret = Maze::NodeType::EXIT;

					} else {
						ret = Maze::NodeType::PORTAL;
					}
				}
				break;

			default:
				ret = Maze::NodeType::EMPTY;
		}

		return ret;
	}

	std::string getId(const Point<int> &pos, char c, Maze::NodeType type) const {
		return getPortal(pos);
	}
};


int main(int argc, char *argv[]) {
	auto inFile = File::readAllLines(argv[1]);

	Maze maze;

	maze.parse(inFile, Callbacks(inFile));
//	maze.draw();

	{
		graph::Graph graph;
		graph::GraphSearchDijkstra dijkstra(&graph);

		Maze::GraphNode *start = maze.getEntrance();
		Maze::GraphNode *end   = maze.getExit();

		maze.fillGraph(graph);

		std::vector<graph::Node *> route;

		dijkstra.search(start->id(), end->id(), route);

		int cost = 0;
		for (int i = 1; i < route.size(); i++) {
			cost += graph.getEdge(route[i - 1]->id(), route[i]->id())->cost();
		}

		PRINTF(("PART_A: %d", cost));
	}
}
