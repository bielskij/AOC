#include <queue>

#include "utils/Maze.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


Maze::Maze() {
	this->yMultiplier = 1;

	this->mapHeight   = 0;
	this->mapWidth    = 0;
	this->map         = nullptr;
}


Maze::~Maze() {

}


struct QueueNode {
	Point<int>       p;
	int              distance;
	Maze::GraphNode *parent;


	QueueNode(const Point<int> &p, int dist, Maze::GraphNode *parent) {
		this->p        = p;
		this->distance = dist;
		this->parent   = parent;
	}
};


void Maze::walkMap(GraphNode *src, bool *visited) {
	const int rowNum[] = { -1,  0, 0, 1 };
	const int colNum[] = {  0, -1, 1, 0 };

	std::queue<QueueNode> q;

	visited[src->position.y() * this->mapWidth + src->position.x()] = true;

	q.push(QueueNode(src->position, 0, src));

	while (! q.empty()) {
		QueueNode curr = q.front(); q.pop();

		switch (this->map[curr.p.y()][curr.p.x()]) {
			case NodeType::KEY:
			case NodeType::DOOR:
				{
					int nodeId = positionToGraphId(curr.p);

					if (curr.parent != this->nodes[nodeId]) {
						this->edges.push_back(new GraphEdge(this->nodes[nodeId], curr.parent, curr.distance));
						this->edges.push_back(new GraphEdge(curr.parent, this->nodes[nodeId], curr.distance));

						curr.distance = 0;
						curr.parent   = this->nodes[nodeId];
					}
				}
				break;

			case NodeType::PORTAL:
				{
					std::pair<GraphNode*, GraphNode *> *pair = nullptr;

					if (curr.p == curr.parent->position) {
						break;
					}

					for (auto p : this->portals) {
						if ((p.second.first && p.second.first->position == curr.p) || (p.second.second && p.second.second->position == curr.p)) {
							pair = &p.second;
							break;
						}
					}

					if (pair) {
						GraphNode *dst = (pair->first->position == curr.p) ? pair->second : pair->first;
						GraphNode *src = (pair->first->position == curr.p) ? pair->first  : pair->second;

						// Node to portal
						if (curr.parent->type == NodeType::EXIT) {
							this->edges.push_back(new GraphEdge(src, curr.parent, curr.distance));
						} else {
							this->edges.push_back(new GraphEdge(curr.parent, src, curr.distance));
						}

						if (curr.parent->type == NodeType::PORTAL) {
							this->edges.push_back(new GraphEdge(src, curr.parent, curr.distance));
						}

						// If destination is known go trough the portal
						if (dst && ! visited[dst->position.y() * this->mapWidth + dst->position.x()]) {
							DBG(("Add edge: %p (%d), %p (%d)", src, src->id(), dst, dst->id()));

							// Both direction
							this->edges.push_back(new GraphEdge(src, dst, 1));
							this->edges.push_back(new GraphEdge(dst, src, 1));

							curr.distance = 0;
							curr.p        = dst->position;
							curr.parent   = dst;

							visited[dst->position.y() * this->mapWidth + dst->position.x()] = true;
						}
					}
				}
				break;

			case NodeType::ENTRANCE:
				{
					int nodeId = positionToGraphId(curr.p);

					if (curr.parent != this->nodes[nodeId]) {
						DBG(("Add edge: %p (%d), %p (%d)", curr.parent, curr.parent->id(), this->nodes[nodeId], this->nodes[nodeId]->id()));

						this->edges.push_back(new GraphEdge(this->nodes[nodeId], curr.parent, curr.distance));

						curr.distance = 0;
						curr.parent   = this->nodes[nodeId];
					}
				}
				break;

			case NodeType::EXIT:
				{
					int nodeId = positionToGraphId(curr.p);

					if (curr.parent != this->nodes[nodeId]) {
						DBG(("Add edge: %p (%d), %p (%d)", curr.parent, curr.parent->id(), this->nodes[nodeId], this->nodes[nodeId]->id()));

						this->edges.push_back(new GraphEdge(curr.parent, this->nodes[nodeId], curr.distance));

						curr.distance = 0;
						curr.parent   = this->nodes[nodeId];
					}
				}
				break;

			default:
				break;
		}

		for (int i = 0; i < 4; i++) {
			int x = curr.p.x() + colNum[i];
			int y = curr.p.y() + rowNum[i];

			if (
				isValidPoint(x, y) &&
				! visited[y * this->mapWidth + x] &&
				(
					this->map[y][x] != NodeType::EMPTY &&
					this->map[y][x] != NodeType::WALL
				)
			) {
				visited[y * this->mapWidth + x] = true;

				q.push(QueueNode(Point<int>(x, y), curr.distance + 1, curr.parent));
			}
		}
	}
}


bool Maze::parse(const std::vector<std::string> &map, const Callbacks &callbacks) {
	bool ret = true;

	this->yMultiplier = 1;
	this->mapHeight   = map.size();
	this->mapWidth    = map[0].length();

	while (this->yMultiplier < map[0].length()) {
		this->yMultiplier *= 10;
	}

	this->map = new NodeType*[this->mapHeight];
	for (int i = 0; i < this->mapHeight; i++) {
		this->map[i] = new NodeType[this->mapWidth];
	}

	this->edges.clear();
	this->nodes.clear();

	for (int row = 0; row < map.size(); row++) {
		for (int col = 0; col < map[row].length(); col++) {
			Point<int> p(col, row);

			int      graphId = this->positionToGraphId(p);
			NodeType nodeType = callbacks.getNodeType(p, map[row][col]);

			switch (nodeType) {
				case NodeType::PORTAL:
					{
						std::string id = callbacks.getId(p, map[row][col], nodeType);

						// Add node
						this->nodes[graphId] = new GraphNode(graphId, p, nodeType);

						// Make connection between portal endpoints
						if (this->portals[id].first == nullptr) {
							this->portals[id].first  = this->nodes[graphId];
						} else {
							this->portals[id].second = this->nodes[graphId];
						}
					}
					break;

				case NodeType::KEY:
				case NodeType::DOOR:
					{
						std::string id = callbacks.getId(p, map[row][col], nodeType);

						this->nodes[graphId] = new GraphNode(graphId, p, nodeType);

						if (nodeType == NodeType::KEY) {
							this->keyDoor[id].first  = this->nodes[graphId];
						} else {
							this->keyDoor[id].second = this->nodes[graphId];
						}
					}
					break;

				case NodeType::EXIT:
				case NodeType::ENTRANCE:
					{
						this->nodes[graphId] = new GraphNode(graphId, p, nodeType);
					}
					break;

				case NodeType::EMPTY:
				case NodeType::PASSAGE:
				case NodeType::WALL:
					break;
			}

			this->map[row][col] = nodeType;
		}
	}

	{
		bool visited[this->mapHeight * this->mapWidth];

		for (int y = 0; y < this->mapHeight; y++) {
			for (int x = 0; x < this->mapWidth; x++) {
				visited[y * this->mapWidth + x] = false;
			}
		}

		GraphNode *src = this->getEntrance();
		if (! src) {
			if (! this->nodes.empty()) {
				src = this->nodes.begin()->second;
			}
		}

		std::vector<GraphNode *> toVisit;

		for (auto n : this->nodes) {
			toVisit.push_back(n.second);
		}

		while (src) {
			this->walkMap(src, visited);

			src = nullptr;

			if (! toVisit.empty()) {
				src = *toVisit.begin(); toVisit.erase(toVisit.begin());

				for (int y = 0; y < this->mapHeight; y++) {
					for (int x = 0; x < this->mapWidth; x++) {
						visited[y * this->mapWidth + x] = false;
					}
				}
			}
		}

		DBG(("Nodes count: %zd, edges: %zd", this->nodes.size(), this->edges.size()));

//		for (int y = 0; y < this->mapHeight; y++) {
//			for (int x = 0; x < this->mapWidth; x++) {
//				printf("%c", visited[y * mapWidth + x] ? 'x' : ' ');
//			}
//			printf("\n");
//		}
//
//		for (auto &n : this->nodes) {
//			DBG(("Node: %d", n.second->id()));
//		}
//
//		for (auto &e : this->edges) {
//			DBG(("Edge: %d - %d (%f)", e->from()->id(), e->to()->id(), e->cost()));
//		}
	}

	return ret;
}


Maze::GraphNode *Maze::getEntrance() {
	GraphNode *ret = nullptr;

	for (auto &n : this->nodes) {
		if (n.second->type == NodeType::ENTRANCE) {
			ret = n.second;
			break;
		}
	}

	return ret;
}


Maze::GraphNode *Maze::getExit() {
	GraphNode *ret = nullptr;

	for (auto &n : this->nodes) {
		if (n.second->type == NodeType::EXIT) {
			ret = n.second;
			break;
		}
	}

	return ret;
}


void Maze::fillGraph(graph::Graph &graph) {
	for (auto &n : this->nodes) {
		graph.addNode(n.second);
	}

	for (auto &e : this->edges) {
		graph.addValidEdge(e);
	}
}


void Maze::draw() {
	for (int y = 0; y < this->mapHeight; y++) {
		for (int x = 0; x < this->mapWidth; x++) {
			switch (this->map[y][x]) {
				case NodeType::DOOR:     printf("D"); break;
				case NodeType::EMPTY:    printf(" "); break;
				case NodeType::ENTRANCE: printf("E"); break;
				case NodeType::EXIT:     printf("X"); break;
				case NodeType::KEY:      printf("K"); break;
				case NodeType::PASSAGE:  printf("."); break;
				case NodeType::PORTAL:   printf("P"); break;
				case NodeType::WALL:     printf("#"); break;
			}
		}

		printf("\n");
	}
}


int Maze::positionToGraphId(const Point<int> &position) const {
	return position.y() * this->yMultiplier + position.x();
}


Point<int> Maze::graphIdToPosition(int graphId) const {
	return Point<int>(graphId % this->yMultiplier, graphId / this->yMultiplier);
}


bool Maze::isValidPoint(int x, int y) {
	return ((x >= 0) && (x < this->mapWidth)) && ((y >= 0) && (y < this->mapHeight));
}


std::vector<Maze::GraphNode *> Maze::getNodes(NodeType type) {
	std::vector<GraphNode *> ret;

	for (auto n : this->nodes) {
		if ((n.second->type & type) != 0) {
			ret.push_back(n.second);
		}
	}

	return ret;
}


std::vector<Maze::GraphEdge *> Maze::getEdges() {
	return this->edges;
}


std::string Maze::getId(GraphNode *node, NodeType type) const {
	std::string ret;

	Point<int> position = this->graphIdToPosition(node->id());
	if (this->map[position.y()][position.x()] == type) {
		auto &list =
			(type == NodeType::PORTAL) ? this->portals :
			(type == NodeType::KEY)    ? this->keyDoor :
			(type == NodeType::DOOR)   ? this->keyDoor :
				std::map<std::string, std::pair<GraphNode *, GraphNode *>>();

		for (const auto &p : list) {
			if ((p.second.first && p.second.first->position == position) || (p.second.second && p.second.second->position == position)) {
				ret = p.first;
				break;
			}
		}
	}

	return ret;
}
