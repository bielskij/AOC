#include <string>
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <algorithm>


namespace graph {
	template <class NodeId>
	class Edge {
		public:
			Edge(const NodeId &src, const NodeId &dst, float cost) {
				this->src  = src;
				this->dst  = dst;
				this->cost = cost;
			}

			const NodeId &getSource() const {
				return this->src;
			}

			const NodeId &getDestination() const {
				return this->dst;
			}

			float getCost() const {
				return this->cost;
			}

		private:
			NodeId src;
			NodeId dst;
			float  cost;
	};

	template <class NodeId>
	class GraphStructureProvider {
		public:
			virtual ~GraphStructureProvider() {}

			virtual std::vector<Edge<NodeId>> getEdges(const NodeId &node) const = 0;
	};

	template <class NodeId>
	class GraphSearchAlgo {
		public:
			virtual ~GraphSearchAlgo() {}

			virtual void search(
				GraphStructureProvider<NodeId> &provider,
				const NodeId                   &start,
				const NodeId                   &end,
				std::vector<NodeId>            &route
			) = 0;
	};

	/*
	 * IMPLEMENTATION
	 */
	template <class NodeId>
	class GraphSearchDijkstra {
		private:
			struct NodeCost {
				NodeId id;
				float  cost;

				public:
					NodeCost(const NodeId &id, float cost) {
						this->id   = id;
						this->cost = cost;
					}

					friend bool operator<(const NodeCost &a, const NodeCost &b) {
						return a.cost > b.cost;
					}
			};

			static constexpr float UNDEFINED = std::numeric_limits<float>::max();

		public:
			void search(
				GraphStructureProvider<NodeId> &provider,
				const NodeId                   &start
			) {
				this->reset();

				this->start = start;

				this->unsettledNodes.push(NodeCost(start, 0));
				this->graphCostToNode[start] = 0;

				while (! this->unsettledNodes.empty()) {
					NodeCost src = this->unsettledNodes.top(); this->unsettledNodes.pop();

					{
						auto edges = provider.getEdges(src.id);

						for (const auto &e : edges) {
							const NodeId &dst = e.getDestination();

							float newCost = this->graphCostToNode[src.id] + e.getCost();

							if (newCost < getCost(dst)) {
								this->graphCostToNode[dst] = newCost;
								this->parent[dst]          = src.id;
							}

							if (this->settledNodes.find(dst) == this->settledNodes.end()) {
								this->settledNodes.insert(dst);

								this->unsettledNodes.push(NodeCost(dst, newCost));
							}
						}
					}
				}
			}

			void reset() {
				this->settledNodes.clear();
				this->parent.clear();
				this->graphCostToNode.clear();

				while (! this->unsettledNodes.empty()) {
					this->unsettledNodes.pop();
				}
			}

			void getRoute(std::vector<NodeId> &route, NodeId end) {
				auto id = end;

				route.clear();

				while (id != start) {
					auto it = this->parent.find(id);

					route.push_back(id);

					if (it == this->parent.end()) {
						route.clear();
						break;
					}

					id = it->second;
				}

				if (id == start) {
					route.push_back(id);
				}

				std::reverse(route.begin(), route.end());
			}

		protected:
			float getCost(const NodeId &id) {
				auto it = graphCostToNode.find(id);
				if (it == graphCostToNode.end()) {
					return UNDEFINED;
				}

				return it->second;
			}

		private:
			std::priority_queue<NodeCost, std::vector<NodeCost>> unsettledNodes;

			NodeId                   start;
			std::set<NodeId>         settledNodes;
			std::map<NodeId, NodeId> parent;
			std::map<NodeId, float>  graphCostToNode;
	};
}
