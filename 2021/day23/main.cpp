#include <unordered_map>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#include "utils/graph/Node.h"
#include "utils/graph/Graph.h"
#include "utils/graph/GraphSearchDijkstra.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class AmphipodsGraph : public graph::Graph {
	public:
		class AmphipodsNode : public graph::Node {
			public:
				class Room;
				class Hallway;

				class Amphipod {
					public:
						Amphipod(char id, const Point<int> &pos, const std::vector<std::string> &map) : _pos(pos), _map(map) {
							this->_id = id;
						}

						char id() const {
							return this->_id;
						}

						const Point<int> &pos() const {
							return this->_pos;
						}

						std::pair<AmphipodsNode *, double> moveTo(const Point<int> &point) {
							std::pair<AmphipodsNode *, double> ret;

							{
								std::vector<std::string> newMap = this->_map;

								newMap[point.y()][point.x()] = this->_id;
								newMap[_pos.y()][_pos.x()]   = '.';

								ret.first  = new AmphipodsNode(newMap, false);
								ret.second = utils::manhattanDistance<int>(this->_pos, point) * this->cost();
							}

							return ret;
						}

						int cost() const {
							int ret = 1;

							for (int i = 'A'; i < this->_id; i++) {
								ret *= 10;
							}

							return ret;
						}

					private:
						char       _id;
						Point<int> _pos;
						const std::vector<std::string> &_map;
				};

				class Room {
					public:
						Room(char id, const std::vector<std::string> &map) : _map(map) {
							this->_id    = id;
							this->_depth = map.size() - 3;

							for (int i = 0; i < this->_depth; i++) {
								Point<int> ampPoint(3 + (2 * (this->_id - 'A')), 2 + i);
								char       ampId = this->_map[ampPoint.y()][ampPoint.x()];

								if (ampId != '.') {
									this->_amphipods.push_back(
										new Amphipod(ampId, ampPoint, this->_map)
									);
								}
							}
						}

						virtual ~Room() {
							auto it = this->_amphipods.begin();

							while (it != this->_amphipods.end()) {
								delete *it;

								it = this->_amphipods.erase(it);
							}
						}

						bool isFull() const {
							return this->_amphipods.size() == this->_depth;
						}

						bool isEmpty() const {
							return this->_amphipods.empty();
						}

						bool isCorrect() const {
							bool ret = true;

							for (auto amp : this->_amphipods) {
								if (amp->id() != this->id()) {
									ret = false;
									break;
								}
							}

							return ret;
						}

						bool isReachableFrom(Amphipod *amphipod) const {
							bool ret = true;

							{
								Point<int> pos   = amphipod->pos();
								Point<int> empty = this->getEmptyPosition();

								// Move to hallway
								if (pos.y() != 1) {
									pos.y(1);
								}

								if (pos.x() > empty.x()) {
									for (int x = pos.x() - 1; x != empty.x(); x--) {
										if (this->_map[pos.y()][x] != '.') {
											ret = false;
											break;
										}
									}

								} else {
									for (int x = pos.x() + 1; x != empty.x(); x++) {
										if (this->_map[pos.y()][x] != '.') {
											ret = false;
											break;
										}
									}
								}
							}

							return ret;
						}

						Amphipod *getFirst() {
							if (! this->_amphipods.empty()) {
								return this->_amphipods.front();
							}

							return nullptr;
						}

						Point<int> getEmptyPosition() const {
							Point<int> ret;

							ret.x(3 + (2 * (this->_id - 'A')));

							for (int y = this->_map.size() - 2; y > 1; y--) {
								if (this->_map[y][ret.x()] == '.') {
									ret.y(y);
									break;
								}
							}

							return ret;
						}

						char id() const {
							return this->_id;
						}

					private:
						const std::vector<std::string> &_map;
						std::vector<Amphipod *>         _amphipods;
						char                            _id;
						int                             _depth;
				};

				class Hallway {
					public:
						Hallway(const std::vector<std::string> &map) : _map(map) {
							for (int i = 0; i < _map[0].length(); i++) {
								Point<int> ampPos(i, 1);
								char       ampId = _map[ampPos.y()][ampPos.x()];

								if ((ampId != '#') && (ampId != '.')) {
									this->_amphipods.push_back(
										new Amphipod(ampId, ampPos, this->_map)
									);
								}
							}
						}

						virtual ~Hallway() {
							auto it = this->_amphipods.begin();

							while (it != this->_amphipods.end()) {
								delete *it;

								it = this->_amphipods.erase(it);
							}
						}

						bool isEmpty() const {
							return this->_amphipods.empty();
						}

						const std::vector<Amphipod *> getAmphipods() const {
							return this->_amphipods;
						}

						std::vector<Point<int>> getReachablePositoins(Amphipod *amphipod) {
							std::vector<Point<int>> ret;

							{
								static const std::set<int> positions = {
									1, 2, 4, 6, 8, 10, 11
								};

								const auto &aPos = amphipod->pos();

								// left
								{
									auto it = positions.find(aPos.x() - 1);

									while (true) {
										if (this->_map[1][*it] == '.') {
											ret.push_back(Point<int>(*it, 1));

										} else {
											break;
										}

										if (it == positions.begin()) {
											break;
										}

										it--;
									}
								}

								// right
								{
									auto it = positions.find(aPos.x() + 1);

									do {
										if (this->_map[1][*it] == '.') {
											ret.push_back(Point<int>(*it, 1));

										} else {
											break;
										}

										it++;
									} while (it != positions.end());
								}
							}

							return ret;
						}

					private:
						const std::vector<std::string> &_map;
						std::vector<Amphipod *>         _amphipods;
				};

			public:
				static const int ID_START = 0;
				static const int ID_END   = 1;

			public:
				AmphipodsNode(const std::vector<std::string> &map, bool start) : _map(map), hallway(_map) {
					this->depth = this->_map.size() - 3;

					for (int i = 0; i < 4; i++) {
						this->_rooms.push_back(new Room('A' + i, this->_map));
					}

					if (start) {
						this->id(ID_START);

					} else if (this->isEnd()) {
						this->id(ID_END);

					} else {
						this->id(ID_GEN++);
					}

//					this->dump();
				}

				virtual ~AmphipodsNode() {
					auto it = this->_rooms.begin();

					while (it != this->_rooms.end()) {
						delete *it;

						it = this->_rooms.erase(it);
					}
				}

				bool isEnd() const {
					bool ret = true;

					for (auto room : this->getRooms()) {
						if (! room->isFull() || ! room->isCorrect()) {
							ret = false;
							break;
						}
					}

					return ret;
				}

				static AmphipodsNode *from(graph::Node *src) {
					if (src != nullptr) {
						return dynamic_cast<AmphipodsNode *>(src);
					}

					return nullptr;
				}

				const std::vector<std::string> &getMap() const {
					return this->_map;
				}

				Hallway *getHallway() {
					return &this->hallway;
				}

				Room *getRoom(char id) {
					for (auto room : this->_rooms) {
						if (room->id() == id) {
							return room;
						}
					}

					return nullptr;
				}

				const std::vector<Room *> getRooms() const {
					return this->_rooms;
				}

				void dump() const {
					for (const auto &row : this->_map) {
						for (const auto &col : row) {
							printf("%c", col);
						}
						printf("\n");
					}
					printf("\n");
				}

				std::string toString() const {
					std::string ret;

					for (const auto &r : this->_map) {
						ret.append(r);
					}

					return ret;
				}

			private:
				static int ID_GEN;

				const std::vector<std::string> _map;

				std::vector<Room *> _rooms;

				Hallway   hallway;
				int       depth;
		};

	public:
		AmphipodsGraph(const std::vector<std::string> &map) : graph::Graph() {
			this->addNode(new AmphipodsNode(map, true));

			{
				std::vector<std::string> endMap = {
					"#############",
					"#...........#",
				};

				for (int i = 0; i < map.size() - 3; i++) {
					endMap.push_back("###A#B#C#D###");
				}

				endMap.push_back("  #########  ");

				this->addNode(new AmphipodsNode(endMap, false));
			}
		}

		void addNewNodeAndEdgeIfNotCached(AmphipodsNode *srcNode, const std::pair<AmphipodsNode *, double> &dstNode) {
			auto dstNodeId = dstNode.first->toString();

			AmphipodsNode *edgeSrc = srcNode;
			AmphipodsNode *edgeDst = nullptr;

			{
				auto cachedNodeIt = this->_nodes.find(dstNodeId);

				if (cachedNodeIt != this->_nodes.end()) {
					edgeDst = cachedNodeIt->second;

					delete dstNode.first;

				} else {
					edgeDst = dstNode.first;

					this->_nodes[dstNodeId] = dstNode.first;
				}
			}

			if (this->getNode(edgeDst->id()) == nullptr) {
				this->addNode(edgeDst);
			}

			if (this->getEdge(edgeSrc->id(), edgeDst->id()) == nullptr) {
				this->addEdge(edgeSrc->id(), edgeDst->id(), dstNode.second);
			}
		}

		virtual std::vector<graph::Edge *> &getEdgeList(int nodeID) {
			auto &ret = Graph::getEdgeList(nodeID);

			DBG(("CALL for id: %d, ret size: %zd", nodeID, ret.size()));

			if (ret.empty()) {
				AmphipodsNode *srcNode = AmphipodsNode::from(this->getNode(nodeID));

				// Hallway to room
				{
					auto hallway = srcNode->getHallway();

					if (! hallway->isEmpty()) {
						auto amphipods = hallway->getAmphipods();

						for (auto amphipod : amphipods) {
							auto room = srcNode->getRoom(amphipod->id());

							if (! room->isFull() && room->isCorrect()) {
								if (room->isReachableFrom(amphipod)) {
									this->addNewNodeAndEdgeIfNotCached(
										srcNode, amphipod->moveTo(room->getEmptyPosition())
									);
								}
							}
						}
					}
				}

				// Room to hallway
				for (auto room : srcNode->getRooms()) {
					if (! room->isCorrect() && ! room->isEmpty()) {
						auto firstAmphipod = room->getFirst();

						for (auto position : srcNode->getHallway()->getReachablePositoins(firstAmphipod)) {
							this->addNewNodeAndEdgeIfNotCached(
								srcNode, firstAmphipod->moveTo(position)
							);
						}
					}
				}

				ret = Graph::getEdgeList(nodeID);
			}

			return ret;
		}

		AmphipodsNode *getAmphipodNode(int id){
			return AmphipodsNode::from(this->getNode(id));
		}

	private:
		std::unordered_map<std::string, AmphipodsNode *> _nodes;
};


int AmphipodsGraph::AmphipodsNode::ID_GEN = 2;


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		AmphipodsGraph graph(lines);

		graph::GraphSearchDijkstra search(&graph);

		{
			std::vector<graph::Node *> route;

			search.search(AmphipodsGraph::AmphipodsNode::ID_START, AmphipodsGraph::AmphipodsNode::ID_END, route, false);

			{
				int cost = 0;

				for (int i = 1; i < route.size(); i++) {
					cost += graph.getEdge(route[i - 1]->id(), route[i]->id())->cost();
				}

				PRINTF(("RESULT: %d", cost));

#if 0
				for (auto node : route) {
					AmphipodsGraph::AmphipodsNode::from(node)->dump();
				}
#endif
			}
		}
	}

	return 0;
}
