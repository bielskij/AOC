#ifndef UTILS_GRAPH_GRAPHSEARCHDFS_H_
#define UTILS_GRAPH_GRAPHSEARCHDFS_H_

#include <vector>
#include <map>
#include <stack>
#include <set>

#include "Graph.h"
#include "GraphSearch.h"


namespace graph {
	class GraphSearchDfs : public GraphSearch {
		protected:
			GraphSearchDfs() {
				this->graph = nullptr;
			}

			void clear(){
				this->settledNodes.clear();
				this->visited.clear();
				this->examinedEdges.clear();
				this->route.clear();

				while (! this->stack.empty()) {
					this->stack.pop();
				}
			}

		public:
			GraphSearchDfs(Graph *graph) {
				this->graph = graph;
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

				// TODO: delete this object!
				this->stack.push(new Edge(start, start, 0));

				while (! this->stack.empty()) {
					Edge *next = this->stack.top(); this->stack.pop();

					this->settledNodes[next->to()->id()] = next->from()->id();
					this->visited.insert(next->to()->id());

					if (next->to()->id() == targetID) {
						int parent = target->id();

						this->route.push_back(target);
						do {
							parent = settledNodes[parent];
							this->route.push_back(graph->getNode(parent));
						} while (parent != startID);

						std::reverse(this->route.begin(), this->route.end());

						route = this->route;

						break;
					}

					auto &nextEdges = graph->getEdgeList(next->to()->id());
					for (auto &e : nextEdges) {
						if (visited.find(e->to()->id()) == visited.end()) {
							this->stack.push(e);

							visited.insert(e->to()->id());

							if (rememberExaminedEdges) {
								this->examinedEdges.push_back(e);
							}
						}
					}
				}
			}

			virtual void getExaminedEdges(std::vector<Edge *> &edges) {
				edges = this->examinedEdges;
			}

			virtual void getRoute(std::vector<Node *> &route) {
				route = this->route;
			}

		protected:
			Graph *graph;

			std::map<int, int>  settledNodes;
			std::set<int>       visited;
			std::vector<Edge *> examinedEdges;
			std::vector<Node *> route;

		private:
			std::stack<Edge *> stack;
	};
}


#endif /* UTILS_GRAPH_GRAPHSEARCHDFS_H_ */
