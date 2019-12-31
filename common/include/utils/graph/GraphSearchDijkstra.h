#ifndef UTILS_GRAPH_GRAPHSEARCHDIJKSTRA_H_
#define UTILS_GRAPH_GRAPHSEARCHDIJKSTRA_H_

#include <limits>
#include <vector>
#include <map>
#include <queue>
#include <set>

#include "Graph.h"
#include "GraphSearch.h"


namespace graph {
	class GraphSearchDijkstra : public GraphSearch {
		private:
			struct NodeCost {
				Node   *node;
				double  cost;

				public:
					NodeCost(Node *node, double cost) {
						this->node = node;
						this->cost = cost;
					}

					bool operator<(const NodeCost &other) {
						return (this->cost < other.cost);
					}

				struct Comparator {
					bool operator()(const NodeCost &lhs, const NodeCost &rhs) {
						return lhs.cost < rhs.cost;
					}
				};
			};

		protected:
			GraphSearchDijkstra() {
				this->graph = nullptr;
			}

		public:
			GraphSearchDijkstra(Graph *graph) {
				this->graph = graph;
			}

			void clear(){
				this->graphCostToNode.clear();
				this->settledNodes.clear();
				this->parent.clear();
				this->examinedEdges.clear();
				this->route.clear();

				while (! this->unsettledNodes.empty()) {
					this->unsettledNodes.pop();
				}
			}

		protected:
			double getCost(Node *node) {
				auto it = graphCostToNode.find(node);
				if (it == graphCostToNode.end()) {
					return std::numeric_limits<double>::max();
				}

				return it->second;
			}

		// inherited methods
		public:
			virtual void search(int startID, int targetID, std::vector<Node *> &route, bool rememberExaminedEdges = false) {
				Node *start  = this->graph->getNode(startID);
				Node *target = this->graph->getNode(targetID);

				if (start == nullptr || target == nullptr) {
					return;
				}

				this->clear();

				route.clear();

				this->unsettledNodes.push(NodeCost(start, 0));
				this->graphCostToNode[start] = 0;

				while (! this->unsettledNodes.empty()) {
					NodeCost pqNext = this->unsettledNodes.top(); this->unsettledNodes.pop();

					Node *next = pqNext.node;
					if (next == target) {
						Node *n = target;

						this->route.push_back(n);
						while (n != start) {
							n = parent[n];
							this->route.push_back(n);
						}

						std::reverse(this->route.begin(), this->route.end());

						route = this->route;

						break;
					}

					this->settledNodes.insert(next);

					for (auto &e : graph->getEdgeList(next->id())) {
						Node *edgeTo = e->to();

						double newCost = getCost(next) + e->cost();

						if (this->settledNodes.find(edgeTo) == this->settledNodes.end() && getCost(edgeTo) > newCost) {
							this->graphCostToNode[edgeTo] = newCost;
							this->parent[edgeTo]          = next;

							this->unsettledNodes.push(NodeCost(edgeTo, newCost));
							if (rememberExaminedEdges) {
								examinedEdges.insert(e);
							}
						}
					}
				}
			}

			virtual void getExaminedEdges(std::vector<Edge *> &edges) {
				edges.clear();

				for (auto &e : this->examinedEdges) {
					edges.push_back(e);
				}
			}

			virtual void getRoute(std::vector<Node *> &route) {
				route = this->route;
			}

		protected:
			Graph *graph;

			std::priority_queue<NodeCost, std::vector<NodeCost>, NodeCost::Comparator> unsettledNodes;

			std::set<Node *>         settledNodes;
			std::map<Node *, Node *> parent;
			std::map<Node *, double> graphCostToNode;
			std::set<Edge *>         examinedEdges;
			std::vector<Node *>      route;
	};
}


#endif /* UTILS_GRAPH_GRAPHSEARCHDIJKSTRA_H_ */
