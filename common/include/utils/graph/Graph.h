#ifndef UTILS_GRAPH_GRAPH_H_
#define UTILS_GRAPH_GRAPH_H_

#include <map>
#include <vector>

#include "Node.h"
#include "Edge.h"

namespace graph {
	class Graph  {
		private:
			class FloatingEdge {
				public:
					int    fromID;
					int    toID;
					Node  *from;
					Node  *to;
					double cost;

					FloatingEdge() {
						this->fromID = -1;
						this->toID   = -1;
						this->from   = nullptr;
						this->to     = nullptr;
						this->cost   = 1.0;
					}

					FloatingEdge(int fromID, int toID, Node *from, Node *to, double cost) {
						this->fromID = fromID;
						this->toID   = toID;
						this->from   = from;
						this->to     = to;
						this->cost   = cost;
					}

					std::string toString() {
	//					String s = "FE ";
	//					s += fromID + ((from == null) ? " (-)" : " (+)");
	//					s += toID + ((to == null) ? " (-)" : " (+)");
	//					s += "    cost= " + cost;
	//					return s;
						return "IMPLEMENT!";
					}
			};

		public:
			Graph() {
				this->nodesFirst = false;
			}

			void addNode(Node *node) {
				this->nodes[node->id()] = node;

				this->resolveFloatEdges(node);
			}

			bool removeNode(int nodeId) {
				Node *node = nodes[nodeId];

				if(node == nullptr) {
					return false;
				}

				edgeLists.erase(node);
				nodes.erase(nodeId);

				std::vector<Edge *> edges;
				std::vector<Edge *> edgesToRemove;

				this->getAllEdges(edges);

				for (auto &e : edges) {
					if (e->to()->id() == nodeId) {
						edgesToRemove.push_back(e);
					}
				}

				for (auto &e : edgesToRemove) {
					auto &list = this->edgeLists[e->from()];

					list.erase(std::find(list.begin(), list.end(), e));
				}

				return true;
			}

			Node *getNode(int id){
				return this->nodes[id];
			}

			bool hasNode(int id){
				return this->nodes[id] != nullptr;
			}

//			Node *getNodeAt(double x, double y, double z, double maxDistance) {
//				double d2 = maxDistance * maxDistance, dx2, dy2, dz2;
//
//				for (auto node = nodes.begin(); node != nodes.end(); node++) {
//					Node *c = node->second;
//
//					dx2 = (c->x() - x) * (c->x() - x);
//					dy2 = (c->y() - y) * (c->y() - y);
//					dz2 = (c->z() - z) * (c->z() - z);
//
//					if(dx2 + dy2 + dz2 < d2) {
//						return node;
//					}
//				}
//
//				return nullptr;
//			}

			int getNbrNodes(){
				return this->nodes.size();
			}

			bool addEdge(int fromID, int toID, double cost){
				Node *fromNode = nodes[fromID];
				Node *toNode   = nodes[toID];
				Edge *ge;

				if (fromNode && toNode) {
					ge = new Edge(fromNode, toNode, cost);

					this->addValidEdge(ge);
					return true;
				}

				FloatingEdge *floatEdge = new FloatingEdge(fromID, toID, fromNode, toNode, cost);

				if (fromNode == nullptr) {
					this->rememberFloatingEdge(fromID, floatEdge);
				}
				if (toNode == nullptr) {
					this->rememberFloatingEdge(toID, floatEdge);
				}

				return false;
			}

			bool addEdge(int fromID, int toID, double costOutward, double costInward){
				bool added = false;

				added  = addEdge(fromID, toID,   costOutward);
				added &= addEdge(toID,   fromID, costInward);

				return added;
			}

			void addValidEdge(Edge *edge){
				Node *fromNode = edge->from();

				this->edgeLists[fromNode].push_back(edge);
			}

			Edge *getEdge(int fromID, int toID) {
				Node *fromNode = nodes[fromID];
				Node *toNode   = nodes[toID];

				if (fromNode == nullptr || toNode == nullptr) {
					return nullptr;
				}

				auto &edgeList = edgeLists[fromNode];
				for (auto &e : edgeList) {
					if (e->to() == toNode) {
						return e;
					}
				}

				return nullptr;
			}

			bool removeEdge(int fromID, int toID) {
				Edge *e = getEdge(fromID, toID);
				if (e){
					Node *fromNode = nodes[fromID];

					auto &list = this->edgeLists[e->from()];

					list.erase(std::find(list.begin(), list.end(), e));

					return true;
				}

				return false;
			}


			double getEdgeCost(int fromID, int toID){
				Edge *e = getEdge(fromID, toID);
				if (e) {
					return e->cost();
				}

				return -1;
			}


			bool hasEdge(int from, int to) {
				return getEdge(from, to) != nullptr;
			}

			std::vector<Edge *> &getEdgeList(int nodeID) {
				return getEdgeList(nodes[nodeID]);
			}

			std::vector<Edge *> &getEdgeList(Node *node) {
				return this->edgeLists[node];
			}

			void getAllEdges(std::vector<Edge *> &edges) {
				edges.clear();

				for (auto &list : this->edgeLists) {
					for (auto &e : list.second) {
						edges.push_back(e);
					}
				}
			}

			void getAllNodes(std::vector<Node *> &nodes) {
				nodes.clear();

				for (auto &p : this->nodes) {
					nodes.push_back(p.second);
				}
			}

			void compact(){
				for (auto &e : this->nodesToBe) {
					for (auto &i : e.second) {
						delete i;
					}
				}

				this->nodesToBe.clear();
			}

		protected:
			void rememberFloatingEdge(int id, FloatingEdge *floatEdge){
				this->nodesToBe[id].push_back(floatEdge);
			}

			void resolveFloatEdges(Node *node){
				int nodeID = node->id();

				auto elist = this->nodesToBe.find(nodeID);
				if (elist != this->nodesToBe.end()) {
					auto it = elist->second.begin();

					while (it != elist->second.end()) {
						FloatingEdge *edge = *it;

						if (edge->fromID == nodeID) {
							edge->from = node;

						} else if (edge->toID == nodeID) {
							edge->to = node;
						}

						if (edge->from && edge->to) {
							this->addValidEdge(new Edge(edge->from, edge->to, edge->cost));

							it = elist->second.erase(it);

						} else {
							it++;
						}
					}

					if (elist->second.empty()) {
						nodesToBe.erase(nodeID);
					}
				}
			}

		protected:
			std::map<int, Node*>                       nodes;
			std::map<Node*, std::vector<Edge *>>       edgeLists;
			std::map<int, std::vector<FloatingEdge *>> nodesToBe;
			bool                                       nodesFirst;
	};
}


#endif /* UTILS_GRAPH_GRAPH_H_ */
