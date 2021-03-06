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


struct CallbacksPart : public Maze::Callbacks {
	std::vector<std::string> *map;
	bool partA;

	CallbacksPart(std::vector<std::string> *map, bool partA) {
		this->map   = map;
		this->partA = partA;
	}

	virtual Maze::NodeType getNodeType(const Point<int> &pos, char c) const {
		Maze::NodeType ret;

		switch (c) {
			case '.':
				if (this->partA) {
					ret = Maze::NodeType::PASSAGE;

				} else {
					if (
						(this->map->at(pos.y() + 1).at(pos.x()    ) == '@') ||
						(this->map->at(pos.y() - 1).at(pos.x()    ) == '@') ||
						(this->map->at(pos.y()    ).at(pos.x() + 1) == '@') ||
						(this->map->at(pos.y()    ).at(pos.x() - 1) == '@')
					) {
						ret = Maze::NodeType::WALL;
					} else if (
						(this->map->at(pos.y() + 1).at(pos.x() + 1) == '@') ||
						(this->map->at(pos.y() + 1).at(pos.x() - 1) == '@') ||
						(this->map->at(pos.y() - 1).at(pos.x() + 1) == '@') ||
						(this->map->at(pos.y() - 1).at(pos.x() - 1) == '@')
					) {
						ret = Maze::NodeType::KEY;
					} else {
						return Maze::NodeType::PASSAGE;
					}
				}
				break;

			case '#':
				ret = Maze::NodeType::WALL;
				break;

			case '@':
				if (this->partA) {
					ret = Maze::NodeType::KEY;
				} else {
					ret = Maze::NodeType::WALL;
				}
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

		if (this->partA) {
			if (c == '@') {
				ret.push_back('1');
			} else {
				ret.push_back(std::toupper(c));
			}

		} else {
			if (this->map->at(pos.y() + 1).at(pos.x() + 1) == '@') {
				ret = '2';
			} else if (this->map->at(pos.y() + 1).at(pos.x() - 1) == '@') {
				ret = '1';
			} else if (this->map->at(pos.y() - 1).at(pos.x() + 1) == '@') {
				ret = '3';
			} else if (this->map->at(pos.y() - 1).at(pos.x() - 1) == '@') {
				ret = '4';

			} else {
				ret.push_back(std::toupper(c));
			}
		}

		return ret;
	}
};


struct KeyRoute {
	int cost;
	int keysOnRoute;
	int doors;

	KeyRoute() {
		this->doors       = 0;
		this->cost        = 0;
		this->keysOnRoute = 0;
	}
};


struct RobotsContext {
	char key[4];
	int  keys;

	std::vector<char> routes[4];

	RobotsContext(char a, char b, char c, char d, int keys) {
		this->key[0] = a;
		this->key[1] = b;
		this->key[2] = c;
		this->key[3] = d;

		this->keys = keys;
	}

	RobotsContext(char a, char b, char c, char d, int keys, const std::vector<char> routes[4]) {
		this->key[0] = a;
		this->key[1] = b;
		this->key[2] = c;
		this->key[3] = d;

		this->keys = keys;

		for (int i = 0; i < 4; i++) {
			this->routes[i] = routes[i];
		}
	}
};


static void dumpkeyMap(std::map<char, std::map<char, KeyRoute>> &keyToKeyMap) {
	for (auto &p : keyToKeyMap) {
		PRINTF(("Key: %c", std::tolower(p.first)));
		for (auto &d : p.second) {
			std::string val;

			{
				char header[32] = { 0 };

				snprintf(header, sizeof(header), "%c: %d,  ", std::tolower(d.first), d.second.cost);

				val.append(header);
			}

			for (int i = 0; i < 25; i++) {
				if (d.second.doors & (1 << i)) {
					val.push_back('A' + i); val += ", ";
				}
			}

			PRINTF(("   %s", val.c_str()));
		}
	}
}


void mazeToKeyToKeyMap(Maze &maze, std::map<char, std::map<char, KeyRoute>> &keyToKeyMap) {
	std::vector<Maze::GraphNode *> keys  = maze.getNodes(Maze::NodeType::KEY);

	graph::Graph g;
	graph::GraphSearchBfs bfs(&g);

	maze.fillGraph(g);

	std::map<char, bool> startPoints;

	for (auto key : keys) {
		std::string id = maze.getId(key, key->type);

		if (id[0] >= '1' && id[0] <= '9') {
			startPoints[id[0]] = true;
		}
	}

	// Find distance between each key
	for (auto key : keys) {
		for (auto other : keys) {
			if (startPoints[maze.getId(other, other->type).at(0)]) {
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

				if (route.size() == 0) {
					continue;
				}

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
					char keyId   = maze.getId(key, key->type).at(0);
					char otherId = maze.getId(other, other->type).at(0);

					keyToKeyMap[keyId][otherId].cost = cost;

					for (auto d : routeDoors) {
						Maze::GraphNode *n = reinterpret_cast<Maze::GraphNode *>(d);

						keyToKeyMap[keyId][otherId].doors |= (1 << (maze.getId(n, n->type).at(0) - 'A'));
					}

					for (auto k : routeKeys) {
						Maze::GraphNode *n = reinterpret_cast<Maze::GraphNode *>(k);

						if (startPoints[maze.getId(n, n->type).at(0)]) {
							continue;
						}

						keyToKeyMap[keyId][otherId].keysOnRoute |= (1 << (maze.getId(n, n->type).at(0) - 'A'));
					}
				}
			}
		}
	}
}


bool nextAllowed(int keys, char key, const KeyRoute &l) {
	if ((keys & (1 << (key - 'A'))) == 0) {
		bool haveAllKeys = true;

		// Check if already have key set for crossed doors
		if ((l.doors & keys) != l.doors) {
			haveAllKeys = false;
		}

		if (haveAllKeys) {
			if ((l.keysOnRoute & keys) != l.keysOnRoute) {
				haveAllKeys = false;
			}
		}

		if (haveAllKeys) {
			return true;
		}
	}

	return false;
}


void dfs(char key, int keys, int allKeys, std::map<char, std::map<char, KeyRoute>> &keyToKeyMap, char *route, int depth, int cost, int *minCost) {
	route[depth] = key;

	cost += keyToKeyMap[route[depth - 1]][route[depth]].cost;

	if (key != '1') {
		keys |= (1 << (key - 'A'));
	}

	if (keys == allKeys) {
		if (cost < *minCost) {
			*minCost = cost;
		}

		return;
	}

	for (auto &next : keyToKeyMap[key]) {
		if (nextAllowed(keys, next.first, next.second)) {
			dfs(next.first, keys, allKeys, keyToKeyMap, route, depth + 1, cost, minCost);
		}
	}
}


void dfs2(char key[4], int keys, int allKeys, std::map<char, std::map<char, KeyRoute>> &keyToKeyMap, int cost, int *minCost) {
	for (int i = 0; i < 4; i++) {
		if (key[i] >= 'A' && key[i] <= 'Z') {
			keys |= (1 << (key[i] - 'A'));
		}
	}

	if (keys == allKeys) {
		if (cost < *minCost) {
			*minCost = cost;
		}

		return;
	}

	{
		std::vector<char> next1;

		next1.push_back(key[0]);
		for (auto &next : keyToKeyMap[key[0]]) {
			if (nextAllowed(keys, next.first, next.second)) {
				next1.push_back(next.first);
			}
		}

		for (auto next1Entry : next1) {
			std::vector<char> next2;

			next2.push_back(key[1]);
			for (auto &next : keyToKeyMap[key[1]]) {
				if (nextAllowed(keys | (1 << (next1Entry - 'A')), next.first, next.second)) {
					next2.push_back(next.first);
				}
			}

			for (auto next2Entry : next2) {
				std::vector<char> next3;

				next3.push_back(key[2]);
				for (auto &next : keyToKeyMap[key[2]]) {
					if (nextAllowed(keys | (1 << (next1Entry - 'A')) | (1 << (next2Entry - 'A')), next.first, next.second)) {
						next3.push_back(next.first);
					}
				}

				for (auto next3Entry : next3) {
					bool anyAdded = false;

					char nextKeys[4];
					int  constCost = 0;

					nextKeys[0] = next1Entry;
					nextKeys[1] = next2Entry;
					nextKeys[2] = next3Entry;

					for (int i = 0; i < 3; i++) {
						if (key[i] != nextKeys[i]) {
							constCost += keyToKeyMap[key[i]][nextKeys[i]].cost;
						}
					}

					for (auto &next : keyToKeyMap[key[3]]) {
						if (nextAllowed(keys | (1 << (next1Entry - 'A')) | (1 << (next2Entry - 'A')) | (1 << (next3Entry - 'A')), next.first, next.second)) {
							nextKeys[3] = next.first;

							dfs2(nextKeys, keys, allKeys, keyToKeyMap, cost + constCost + keyToKeyMap[key[3]][nextKeys[3]].cost, minCost);

							anyAdded = true;
						}
					}

					if (! anyAdded) {
						if (next1Entry != key[0] || next2Entry != key[1] || next3Entry != key[2]) {
							nextKeys[3] = key[3];

							dfs2(nextKeys, keys, allKeys, keyToKeyMap, cost + constCost, minCost);
						}
					}
				}
			}
		}
	}
}


int main(int argc, char *argv[]) {
	std::vector<std::string> mapString = File::readAllLines(argv[1]);

	{
		Maze maze;

		maze.parse(mapString, CallbacksPart(&mapString, true));
//		maze.draw();

		std::map<char, std::map<char, KeyRoute>> keyToKeyMap;

		mazeToKeyToKeyMap(maze, keyToKeyMap);

//		dumpkeyMap(keyToKeyMap);

		{
			char route[25];
			int minCost = std::numeric_limits<int>::max();

			dfs('1', 0, (1 << (maze.getNodes(Maze::NodeType::KEY).size() - 1)) - 1, keyToKeyMap, route, 0, 0, &minCost);

			PRINTF(("PART_A: %d", minCost));
		}
	}

	{
		Maze maze;

		maze.parse(mapString, CallbacksPart(&mapString, false));
//		maze.draw();

		std::map<char, std::map<char, KeyRoute>> keyToKeyMap;

		mazeToKeyToKeyMap(maze, keyToKeyMap);

//		dumpkeyMap(keyToKeyMap);

		char key[4];

		key[0] = '1';
		key[1] = '2';
		key[2] = '3';
		key[3] = '4';

		int minKey = std::numeric_limits<int>::max();
		std::vector<char> routes[4];

		dfs2(key, 0, (1 << (maze.getNodes(Maze::NodeType::KEY).size() - 4)) - 1, keyToKeyMap, 0, &minKey);

		PRINTF(("PART_B: %d", minKey));
	}
}
