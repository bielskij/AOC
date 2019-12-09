#include <iostream>
#include <map>
#include <stack>
#include <queue>
#include <limits.h>

#include "utils/utils.h"
#include "utils/file.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"


struct Node {
	std::vector<Node *> neighbours;

	Node() {
	}
};


static void _dijkstra(std::vector<Node *> &nodes, Node *root, std::map<Node *, int> &dist, std::map<Node *, Node *> &prev) {
	std::vector<Node *> q;

	dist.clear();
	prev.clear();

	for (auto v = nodes.begin(); v != nodes.end(); v++) {
		dist[*v] = INT_MAX;
		prev[*v] = NULL;

		q.push_back(*v);
	}

	dist[root] = 0;

	while (! q.empty()) {
		Node *u;

		{
			std::vector<Node *>::iterator uIt = q.begin();

			for (auto it = q.begin(); it != q.end(); it++) {
				if (dist[*it] < dist[*uIt]) {
					uIt = it;
				}
			}

			u = *uIt;

			q.erase(uIt);
		}

		for (auto v = u->neighbours.begin(); v != u->neighbours.end(); v++) {
			int alt = dist[u] + 1;

			if (alt < dist[*v]) {
				dist[*v] = alt;
				prev[*v] = u;
			}
		}
	}
}


static void _bfs(Node *root, std::map<Node *, int> &dist) {
	std::map<Node *, bool> visited;

	std::queue<Node *> q;

	dist[root] = 0;
	visited[root] = true;

	q.push(root);

	while (! q.empty()) {
		Node *x = q.front();

		q.pop();

		for (auto child = x->neighbours.begin(); child != x->neighbours.end(); child++) {
			if (visited[*child]) {
				continue;
			}

			dist[*child] = dist[x] + 1;
			q.push(*child);
			visited[*child] = true;
		}
	}
}


int main(int argc, char *argv[]) {
	std::map<std::string, Node *> objectsMap;

	{
		std::vector<std::string> map = File::readAllLines(argv[1]);

		for (auto it = map.begin(); it != map.end(); it++) {
			std::vector<std::string> entry = utils::strTok(*it, ')');

			if (objectsMap.find(entry[0]) == objectsMap.end()) {
				objectsMap[entry[0]] = new Node();
			}

			if (objectsMap.find(entry[1]) == objectsMap.end()) {
				objectsMap[entry[1]] = new Node();
			}
		}

		for (auto it = map.begin(); it != map.end(); it++) {
			std::vector<std::string> entry = utils::strTok(*it, ')');

			Node *l = objectsMap.find(entry[0])->second;
			Node *r  = objectsMap.find(entry[1])->second;

			l->neighbours.push_back(r);
			r->neighbours.push_back(l);
		}
	}

	{
		Node *root = objectsMap["COM"];

		std::map<Node *, int> dist;

		_bfs(root, dist);

		int sum = 0;
		for (auto it = dist.begin(); it != dist.end(); it++) {
			sum += it->second;
		}

		PRINTF(("PART A: %d", sum));
	}

	{
		std::vector<Node *> nodes;

		std::map<Node *, int>    dist;
		std::map<Node *, Node *> prev;

		for (auto it = objectsMap.begin(); it != objectsMap.end(); it++) {
			nodes.push_back(it->second);
		}

		_dijkstra(nodes, objectsMap["YOU"], dist, prev);

		PRINTF(("PART B: %d", dist[objectsMap["SAN"]] - 2));
	}
}
