#include <iostream>
#include <cstring>
#include <cassert>
#include <climits>
#include <algorithm>
#include <functional>
#include <unistd.h>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/Maze.h"
#include "utils/graph/Graph.h"
#include "utils/graph/GraphSearchBfs.h"

#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Callbacks : public Maze::Callbacks {
	std::vector<std::string> *map;

	Callbacks(std::vector<std::string> *map) {
		this->map = map;
	}

	virtual Maze::NodeType getNodeType(const Point<int> &pos, char c) const {
		Maze::NodeType ret;

		switch (c) {
			case '.':
				ret = Maze::NodeType::PASSAGE;
				break;

			case '#':
				ret = Maze::NodeType::WALL;
				break;

			case '@':
				ret = Maze::NodeType::ENTRANCE;
				break;

			default:
				if (c >= 'A' && c <= 'Z') {
					ret = Maze::NodeType::DOOR;

				} else if (c >= 'a' && c <= 'z') {
					ret = Maze::NodeType::KEY;

				} else {
					ret = Maze::NodeType::EMPTY;
				}
		}

		return ret;
	}

	virtual std::string getId(const Point<int> &pos, char c, Maze::NodeType type) const {
		std::string ret;

		ret.push_back(std::toupper(c));

		return ret;
	}
};


struct KeyRoute {
	int               cost;
	int               keysOnRoute;
	std::vector<char> doors;

	KeyRoute() {
		this->cost        = 0;
		this->keysOnRoute = 0;
	}
};


void dfs(char key, int keys, int allKeys, std::map<char, std::map<char, KeyRoute>> &keyToKeyMap, char *route, int depth, int cost, int *minCost) {
	route[depth] = key;

	cost += keyToKeyMap[route[depth - 1]][route[depth]].cost;

	if (key != '@') {
		keys |= (1 << (key - 'A'));
	}

	if (keys == allKeys) {
		if (cost < *minCost) {
			*minCost = cost;
		}

//		for (int i = 0; i <= depth; i++) {
//			printf("%c, ", route[i]);
//		}
//		printf("\n");
		return;
	}

	for (auto &next : keyToKeyMap[key]) {
		if ((keys & (1 << (next.first - 'A'))) == 0) {
			bool haveAllKeys = true;

			// Check if already have key set for crossed doors
			for (auto &door : next.second.doors) {
				if ((keys & (1 << (door - 'A'))) == 0) {
					haveAllKeys = false;
					break;
				}
			}

			if (haveAllKeys) {
				if ((next.second.keysOnRoute & keys) != next.second.keysOnRoute) {
					haveAllKeys = false;
				}
			}

			if (haveAllKeys) {
				dfs(next.first, keys, allKeys, keyToKeyMap, route, depth + 1, cost, minCost);
			}
		}
	}
}


int main(int argc, char *argv[]) {
	std::vector<std::string> mapString = File::readAllLines(argv[1]);

	Maze maze;

	maze.parse(mapString, Callbacks(&mapString));
//	maze.draw();

	{
		std::vector<Maze::GraphNode *> keys  = maze.getNodes(Maze::NodeType::KEY);

		std::map<char, std::map<char, KeyRoute>> keyToKeyMap;

		graph::Graph g;
		graph::GraphSearchBfs bfs(&g);

		maze.fillGraph(g);

		// Treat entrance as a key
		keys.push_back(maze.getEntrance());

		// Find distance between each key
		for (auto key : keys) {
			for (auto other : keys) {
				if (other == maze.getEntrance()) {
					continue;
				}

				if (other == key) {
					continue;
				}

				{
					std::vector<graph::Node *> route;
					std::vector<graph::Node *> routeDoors;
					std::vector<graph::Node *> routeKeys;

					bfs.search(key->id(), other->id(), route);

//					PRINTF(("%s -> %s (num: %zd)", maze.getId(key, Maze::NodeType::KEY).c_str(), maze.getId(other, Maze::NodeType::KEY).c_str(), route.size()));

					int cost = 0;
					for (size_t i = 1; i < route.size(); i++) {
						Maze::GraphNode *n = reinterpret_cast<Maze::GraphNode *>(route.at(i));

						switch (n->type) {
							case Maze::NodeType::DOOR:
								routeDoors.push_back(n);
								break;

							case Maze::NodeType::KEY:
								if (i != route.size() - 1) {
									routeKeys.push_back(n);
								}
								break;

							default:
								break;
						}

						cost += g.getEdge(route[i - 1]->id(), route[i]->id())->cost();
					}

					{
						char keyId   = key == maze.getEntrance() ? '@' : maze.getId(key,   key->type).at(0);
						char otherId = maze.getId(other, other->type).at(0);

						keyToKeyMap[keyId][otherId].cost = cost;

						for (auto d : routeDoors) {
							Maze::GraphNode *n = reinterpret_cast<Maze::GraphNode *>(d);

							keyToKeyMap[keyId][otherId].doors.push_back(maze.getId(n, n->type).at(0));
						}

						for (auto k : routeKeys) {
							Maze::GraphNode *n = reinterpret_cast<Maze::GraphNode *>(k);

							keyToKeyMap[keyId][otherId].keysOnRoute |= (1 << (maze.getId(n, n->type).at(0) - 'A'));
						}
					}
				}
			}
		}

#if 1
		for (auto &p : keyToKeyMap) {
			PRINTF(("Key: %c", std::tolower(p.first)));
			for (auto &d : p.second) {
				std::string val;

				{
					char header[32] = { 0 };

					snprintf(header, sizeof(header), "%c: %d,  ", std::tolower(d.first), d.second.cost);

					val.append(header);
				}

				for (auto &door : d.second.doors) {
					val.push_back(door); val += ", ";
				}

				PRINTF(("   %s", val.c_str()));
			}
		}
#endif

		{
			char route[25];
			int minCost = std::numeric_limits<int>::max();

			dfs('@', 0, (1 << (keys.size() - 1)) - 1, keyToKeyMap, route, 0, 0, &minCost);

			PRINTF(("PART_A: %d", minCost));
		}
	}
}
