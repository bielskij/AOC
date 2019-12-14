#include <iostream>
#include <map>
#include <stack>
#include <queue>
#include <limits.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"


class Object : public Graph::Vertex {
	public:
		Object(const std::string &name) : Graph::Vertex(), name(name) {
		}

		std::string getLabel() {
			return name;
		}

	private:
		std::string name;
};


int main(int argc, char *argv[]) {
	std::map<std::string, Object *> objectsMap;

	std::vector<Graph::Edge> edges;

	{
		std::vector<std::string> map = File::readAllLines(argv[1]);

		for (auto it = map.begin(); it != map.end(); it++) {
			std::vector<std::string> entry = utils::strTok(*it, ')');

			if (objectsMap.find(entry[0]) == objectsMap.end()) {
				objectsMap[entry[0]] = new Object(entry[0]);
			}

			if (objectsMap.find(entry[1]) == objectsMap.end()) {
				objectsMap[entry[1]] = new Object(entry[1]);
			}
		}

		for (auto it = map.begin(); it != map.end(); it++) {
			std::vector<std::string> entry = utils::strTok(*it, ')');

			Object *l = objectsMap.find(entry[0])->second;
			Object *r = objectsMap.find(entry[1])->second;

			edges.push_back(Graph::Edge(l, r, 1, true));
		}
	}

	{
		Graph g(edges);

		Object *root = objectsMap["COM"];

		std::map<Graph::Vertex *, float> dist = g.bfs(root);

		int sum = 0;
		for (auto it = dist.begin(); it != dist.end(); it++) {
			sum += it->second;
		}

		PRINTF(("PART A: %d", sum));
	}

	{

		std::map<Graph::Vertex *, float>           dist;
		std::map<Graph::Vertex *, Graph::Vertex *> prev;

		Graph g(edges);

		g.dijkstra(objectsMap["YOU"], dist, prev);

		PRINTF(("PART B: %d", (int)dist[objectsMap["SAN"]] - 2));
	}
}
