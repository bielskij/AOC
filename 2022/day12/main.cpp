#include <cstring>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph/Graph.h"
#include "utils/graph/GraphSearchDijkstra.h"
#include "utils/graph/GraphSearchBfs.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int point2Id(int x, int y) {
	return (y << 16) | x;
}


static Point<int> id2Point(int id) {
	return Point<int>(id & 0xffff, id >> 16);
}


static void dumpRoute(std::vector<graph::Node *> route, const std::vector<std::string> &map) {
	char res[map.size()][map[0].size()];

	memset(res, '.', map.size() * map[0].size());

	for (size_t i = 1; i < route.size(); i++) {
		Point<int> from = id2Point(route[i - 1]->id());
		Point<int> to   = id2Point(route[i    ]->id());

		char d;
		if (to.x() > from.x()) {
			d = '>';
		} else if (to.x() < from.x()) {
			d = '<';
		} else if (to.y() > from.y()) {
			d = 'v';
		} else {
			d = '^';
		}

		res[from.y()][from.x()] = d;
	}

	for (int y = 0; y < map.size(); y++) {
		for (int x = 0; x < map[0].size(); x++) {
			printf("%c", res[y][x]);
		}
		printf("\n");
	}
}


int main(int argc, char *argv[]) {
	auto map = File::readAllLines(argv[1]);

	{
		int width  = map[0].length();
		int height = map.size();

		graph::Graph graph;

		Point<int> endPoint;
		Point<int> startPoint;

		std::vector<Point<int>> allStartPoints;

		{
			const int  rowNum[] = { 1, -1,  0, 0 };
			const int  colNum[] = { 0,  0, -1, 1 };

			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int currNodeId = point2Id(x, y);

					graph::Node *currNode = graph.getNode(currNodeId);
					if (currNode == nullptr) {
						currNode = new graph::Node(point2Id(x, y));

						graph.addNode(currNode);
					}

					switch (map[y][x]) {
						case 'S':
							startPoint = Point<int>(x, y);
							break;

						case 'E':
							endPoint = Point<int>(x, y);
							break;

						case 'a':
							allStartPoints.push_back(Point<int>(x, y));
							break;
					}

					for (int i = 0; i < 4; i++) {
						int nx = x + colNum[i];
						int ny = y + rowNum[i];

						if ((nx >= 0) && (nx < width) && (ny >= 0) && (ny < height)) {
							int  neighbourId  = point2Id(nx, ny);

							char elevationSrc = map[y][x];
							char elevationDst = map[ny][nx];

							if (elevationSrc == 'S') {
								elevationSrc = 'a';

							} else if (elevationSrc == 'E') {
								elevationSrc = 'z';
							}

							if (elevationDst == 'S') {
								elevationDst = 'a';

							} else if (elevationDst == 'E') {
								elevationDst = 'z';
							}

							if (
								elevationDst > elevationSrc &&
								std::abs(elevationDst - elevationSrc) > 1
							) {
								continue;
							}

							graph::Node *neighbour = graph.getNode(neighbourId);
							if (neighbour == nullptr) {
								neighbour = new graph::Node(neighbourId);

								graph.addNode(neighbour);
							}

							graph.addEdge(currNode->id(), neighbour->id(), 1);
						}
					}
				}
			}

			allStartPoints.push_back(startPoint);
		}

		{
			graph::GraphSearchBfs bfs(&graph);

			std::vector<graph::Node *> route;

			bfs.search(point2Id(startPoint.x(), startPoint.y()), point2Id(endPoint.x(), endPoint.y()), route);

			dumpRoute(route, map);

			PRINTF(("PART_A: %zd", route.size() - 1));
		}

		{
			graph::GraphSearchBfs bfs(&graph);

			std::vector<graph::Node *> minRoute;

			for (auto &p : allStartPoints) {
				std::vector<graph::Node *> route;

				bfs.search(point2Id(p.x(), p.y()), point2Id(endPoint.x(), endPoint.y()), route);

				if (minRoute.empty() || (! route.empty() && route.size() < minRoute.size())) {
					minRoute = route;
				}
			}

			dumpRoute(minRoute, map);

			PRINTF(("PART_B: %d", minRoute.size() - 1));
		}
	}

	return 0;
}
