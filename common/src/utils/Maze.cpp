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

	for (int row = 0; row < map.size(); row++) {
		for (int col = 0; col < map[row].length(); col++) {
			Point<int> p(col, row);

			this->map[row][col] = callbacks.getNodeType(p, map[row][col]);

			switch (this->map[row][col]) {
				case NodeType::PORTAL:
					{
						std::string id = callbacks.getId(p, map[row][col], this->map[row][col]);

						if (this->map[row][col] == NodeType::PORTAL) {
							if (this->portals.find(id) == this->portals.end()) {
								this->portals[id].first  = p;
							} else {
								this->portals[id].second = p;
							}
						}
					}
					break;

				case NodeType::KEY:
				case NodeType::DOOR:
					{
						ERR(("TODO: IMPLEMENT"));
					}
					break;

				case NodeType::ENTRANCE:
					{
						this->entrance = p;
					}
					break;

				case NodeType::EXIT:
					{
						this->exit = p;
					}
					break;

				case NodeType::EMPTY:
				case NodeType::PASSAGE:
				case NodeType::WALL:
					break;
			}
		}
	}

	this->edges.clear();
	this->nodes.clear();

	// Add entrance node
	if (this->map[this->entrance.y()][this->entrance.x()] == NodeType::ENTRANCE) {
		this->nodes[positionToGraphId(this->entrance)] = new GraphNode(positionToGraphId(this->entrance), this->entrance, NodeType::ENTRANCE);
	}

	{
		bool visited[this->mapHeight][this->mapWidth];

		const int rowNum[] = { -1,  0, 0, 1};
		const int colNum[] = {  0, -1, 1, 0};

		for (int y = 0; y < this->mapHeight; y++) {
			for (int x = 0; x < this->mapWidth; x++) {
				visited[y][x] = false;
			}
		}

		visited[this->entrance.y()][this->entrance.x()] = true;

		std::queue<QueueNode> q;

		q.push(QueueNode(this->entrance, 0, this->nodes[positionToGraphId(this->entrance)]));

		while (! q.empty()) {
			QueueNode curr = q.front(); q.pop();

//			DBG(("Current: (%d, %d)", curr.p.x(), curr.p.y()));

			switch (this->map[curr.p.y()][curr.p.x()]) {
				// TODO: Add support for door/key pair
				case NodeType::PORTAL:
					{
						std::pair<Point<int>, Point<int>> *pair = nullptr;

						for (auto p : this->portals) {
							if (p.second.first == curr.p || p.second.second == curr.p) {
								pair = &p.second;
								break;
							}
						}

						if (pair) {
							Point<int> dest = pair->first == curr.p ? pair->second : pair->first;

							if (! visited[dest.y()][dest.x()]) {
								int nodeId = positionToGraphId(dest);
								if (this->nodes[nodeId] == nullptr) {
									this->nodes[nodeId] = new GraphNode(nodeId, dest, NodeType::PORTAL);
								}

								DBG(("Add edge: %p (%d), %p (%d)", curr.parent, curr.parent->id(), this->nodes[nodeId], this->nodes[nodeId]->id()));

								this->edges.push_back(new GraphEdge(curr.parent, this->nodes[nodeId], curr.distance));

								curr.distance = 1;
								curr.p        = dest;
								curr.parent   = this->nodes[nodeId];

								visited[dest.y()][dest.x()] = true;
							}
						}
					}
					break;

				case NodeType::EXIT:
					{
						int nodeId = positionToGraphId(curr.p);
						if (this->nodes[nodeId] == nullptr) {
							this->nodes[nodeId] = new GraphNode(nodeId, curr.p, NodeType::EXIT);
						}

						DBG(("Add edge: %p (%d), %p (%d)", curr.parent, curr.parent->id(), this->nodes[nodeId], this->nodes[nodeId]->id()));

						this->edges.push_back(new GraphEdge(curr.parent, this->nodes[nodeId], curr.distance));

						curr.distance = 0;
						curr.parent   = this->nodes[nodeId];
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
					! visited[y][x] &&
					(
						this->map[y][x] != NodeType::EMPTY &&
						this->map[y][x] != NodeType::WALL
					)
				) {
					visited[y][x] = true;

					q.push(QueueNode(Point<int>(x, y), curr.distance + 1, curr.parent));
				}
			}
		}

		DBG(("Nodes count: %zd, edges: %zd", this->nodes.size(), this->edges.size()));

//		for (int y = 0; y < this->mapHeight; y++) {
//			for (int x = 0; x < this->mapWidth; x++) {
//				printf("%c", visited[y][x] ? 'x' : ' ');
//			}
//			printf("\n");
//		}
	}

	return ret;
}


Maze::GraphNode *Maze::getEntrance() {
	int nodeId = positionToGraphId(this->entrance);

	if (this->nodes.find(nodeId) != this->nodes.end()) {
		return this->nodes[nodeId];
	}

	return nullptr;
}


Maze::GraphNode *Maze::getExit() {
	int nodeId = positionToGraphId(this->exit);

	if (this->nodes.find(nodeId) != this->nodes.end()) {
		return this->nodes[nodeId];
	}

	return nullptr;
}


void Maze::fillGraph(graph::Graph &graph) {
	for (auto &n : this->nodes) {
		DBG(("Node: %d", n.second->id()));
		graph.addNode(n.second);
	}

	for (auto &e : this->edges) {
		DBG(("Edge: %d - %d (%f)", e->from()->id(), e->to()->id(), e->cost()));
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


int Maze::positionToGraphId(const Point<int> &position) {
	return position.y() * this->yMultiplier + position.x();
}


Point<int> Maze::graphIdToPosition(int graphId) {
	return Point<int>(graphId % this->yMultiplier, graphId / this->yMultiplier);
}


bool Maze::isValidPoint(int x, int y) {
	return ((x >= 0) && (x < this->mapWidth)) && ((y >= 0) && (y < this->mapHeight));
}
