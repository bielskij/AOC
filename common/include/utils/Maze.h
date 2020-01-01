#ifndef UTILS_MAZE_H_
#define UTILS_MAZE_H_

#include "common/types.h"
#include "utils/graph/Graph.h"

class Maze {
	public:
		enum NodeType {
			EMPTY,

			WALL,
			PASSAGE,
			ENTRANCE,
			EXIT,

			PORTAL,
			KEY,
			DOOR
		};

		struct GraphNode : public graph::Node {
			GraphNode(int id, const Point<int> &pos, NodeType type) : graph::Node(id) {
				this->position = pos;
				this->type     = type;
			}

			Point<int> position;
			NodeType   type;
		};

		struct GraphEdge : public graph::Edge {
			GraphEdge(GraphNode *src, GraphNode *dst, double cost) : graph::Edge(src, dst, cost) {

			}
		};

		struct Callbacks {
			// Maps node type form char
			virtual NodeType getNodeType(const Point<int> &pos, char c) const = 0;

			// Returns identifier for logical object pair like key/door or portals
			virtual std::string getId(const Point<int> &pos, char c, NodeType type) const = 0;
		};

	public:
		Maze();
		virtual ~Maze();

		bool parse(const std::vector<std::string> &map, const Callbacks &callbacks);
		void draw();

		GraphNode *getEntrance();
		GraphNode *getExit();
		void fillGraph(graph::Graph &graph);

	private:
		int positionToGraphId(const Point<int> &position);
		Point<int> graphIdToPosition(int graphId);

		bool isValidPoint(int x, int y);

	private:
		int        mapWidth;
		int        mapHeight;
		NodeType **map;
		int yMultiplier;

		std::map<std::string, std::pair<Point<int>, Point<int>>> portals;

		std::map<int, GraphNode *> nodes;
		std::vector<GraphEdge *>   edges;

		Point<int>   entrance;
		Point<int>   exit;
};

#endif /* UTILS_MAZE_H_ */
