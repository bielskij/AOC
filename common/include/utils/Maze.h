#ifndef UTILS_MAZE_H_
#define UTILS_MAZE_H_

#include "common/types.h"
#include "utils/graph/Graph.h"

class Maze {
	public:
		enum NodeType {
			EMPTY    = 1 << 0,

			WALL     = 1 << 1,
			PASSAGE  = 1 << 2,
			ENTRANCE = 1 << 3,
			EXIT     = 1 << 4,

			PORTAL   = 1 << 5,
			KEY      = 1 << 6,
			DOOR     = 1 << 7,

			ALL = 0xffffffff
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
			virtual ~Callbacks() {
			}

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

		void fillGraph(graph::Graph &graph);

		GraphNode *getEntrance();
		GraphNode *getExit();
		std::vector<GraphNode *> getNodes(NodeType type = NodeType::ALL);
		std::vector<GraphEdge *> getEdges();

		std::string getId(GraphNode *node, NodeType type) const;

	private:
		int positionToGraphId(const Point<int> &position) const;
		Point<int> graphIdToPosition(int graphId) const;

		bool isValidPoint(int x, int y);

		void walkMap(GraphNode *src, bool *visited);

	private:
		int        mapWidth;
		int        mapHeight;
		NodeType **map;
		int yMultiplier;

		std::map<std::string, std::pair<GraphNode *, GraphNode *>> portals;
		std::map<std::string, std::pair<GraphNode *, GraphNode *>> keyDoor;

		std::map<int, GraphNode *> nodes;
		std::vector<GraphEdge *>   edges;
};

#endif /* UTILS_MAZE_H_ */
