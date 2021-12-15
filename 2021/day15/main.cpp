#include <queue>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph/Graph.h"
#include "utils/graph/GraphSearchDijkstra.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int point2Id(int x, int y) {
	return (y << 16) | x;
}


static int solve(const std::vector<std::string> &map) {
	int width  = map[0].length();
	int height = map.size();

	graph::Graph graph;

	{
		const int rowNum[] = { -1,  0, 0, 1 };
		const int colNum[] = {  0, -1, 1, 0 };

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int currNodeId = point2Id(x, y);

				graph::Node *currNode = graph.getNode(currNodeId);
				if (currNode == nullptr) {
					currNode = new graph::Node(point2Id(x, y));

					graph.addNode(currNode);
				}

				for (int i = 0; i < 4; i++) {
					int nx = x + colNum[i];
					int ny = y + rowNum[i];

					if ((nx >= 0) && (nx < width) && (ny >= 0) && (ny < height)) {
						int neighbourId = point2Id(nx, ny);

						graph::Node *neighbour = graph.getNode(neighbourId);
						if (neighbour == nullptr) {
							neighbour = new graph::Node(neighbourId);

							graph.addNode(neighbour);
						}

						graph.addEdge(currNode->id(), neighbour->id(), map[ny][nx] - '0');
					}
				}
			}
		}
	}

	graph::GraphSearchDijkstra dijkstra(&graph);

	std::vector<graph::Node *> route;

	dijkstra.search(point2Id(0, 0), point2Id(width - 1, height - 1), route);

	if (! route.empty()) {
		int cost = 0;

		for (int i = 1; i < route.size(); i++) {
			cost += graph.getEdge(route[i - 1]->id(), route[i]->id())->cost();
		}

		return cost;
	}

	return 0;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines)));

	{
		std::vector<std::string> map = lines;

		for (int multiplier = 1; multiplier < 5; multiplier++) {
			for (int y = 0; y < lines.size(); y++) {
				for (int x = 0; x < lines[0].size(); x++) {
					int val = (lines[y][x] - '0') + multiplier;

					if (val > 9) {
						val -= 9;
					}

					map[y].push_back(val + '0');
				}
			}
		}

		int mapHeight = map.size();

		for (int multiplier = 1; multiplier < 5; multiplier++) {
			for (int y = 0; y < mapHeight; y++) {
				std::string line = map[y];

				for (int x = 0; x < line.size(); x++) {
					int val = (line[x] - '0') + multiplier;

					if (val > 9) {
						val -= 9;
					}

					line[x] = val + '0';
				}

				map.push_back(line);
			}
		}

		PRINTF(("PART_B: %d", solve(map)));
	}

	return 0;
}
