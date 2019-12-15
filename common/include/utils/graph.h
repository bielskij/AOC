#ifndef UTILS_GRAPH_H_
#define UTILS_GRAPH_H_

#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <set>

class Graph {
	public:
		class Vertex {
			public:
				Vertex() {
				}

				virtual ~Vertex() {
				}

				virtual std::string getLabel() = 0;
		};

		class Edge {
			public:
				Edge(Vertex *src, Vertex *dst, float weight, bool bothDirections) {
					this->src            = src;
					this->dst            = dst;
					this->weight         = weight;
					this->bothDirections = bothDirections;
				}

				Vertex *getSrc() const {
					return this->src;
				}

				Vertex *getDst() const {
					return this->dst;
				}

				float getWeight() const {
					return this->weight;
				}

				bool isBothDirection() const {
					return this->bothDirections;
				}

			private:
				Vertex *src;
				Vertex *dst;
				float   weight;
				bool    bothDirections;
		};

		Graph(const std::vector<Edge> &edges) {
			for (auto edge = edges.begin(); edge != edges.end(); edge++) {
				this->vertexes.insert(edge->getSrc());
				this->vertexes.insert(edge->getDst());

				// TODO: Check for duplicates
				this->adjVertices[edge->getSrc()].push_back(std::pair<Vertex *, float>(edge->getDst(), edge->getWeight()));

				if (edge->isBothDirection()) {
					this->adjVertices[edge->getDst()].push_back(std::pair<Vertex *, float>(edge->getSrc(), edge->getWeight()));
				}
			}
		}

		std::map<Vertex *, float> bfs(Vertex *root) {
			std::map<Vertex *, float> ret;

			{
				std::map<Vertex *, bool> visited;
				std::queue<Vertex *> queue;

				ret[root]     = 0;
				visited[root] = true;

				queue.push(root);

				while (! queue.empty()) {
					Vertex *vertex = queue.front(); queue.pop();

					for (auto v = adjVertices[vertex].begin(); v != adjVertices[vertex].end(); v++) {
						if (! visited[v->first]) {
							ret[v->first] = ret[vertex] + v->second;
							queue.push(v->first);
							visited[v->first] = true;
						}
					}
				}
			}

			return ret;
		}

		void dijkstra(Vertex *root, std::map<Vertex *, float> &dist, std::map<Vertex *, Vertex *> &prev) {
			std::vector<Vertex *> q;

			dist.clear();
			prev.clear();

			for (auto v = vertexes.begin(); v != vertexes.end(); v++) {
				dist[*v] = INT_MAX;
				prev[*v] = NULL;

				q.push_back(*v);
			}

			dist[root] = 0;
			prev[root] = nullptr;

			while (! q.empty()) {
				Vertex *u;

				{
					std::vector<Vertex *>::iterator uIt = q.begin();

					for (auto it = q.begin(); it != q.end(); it++) {
						if (dist[*it] < dist[*uIt]) {
							uIt = it;
						}
					}

					u = *uIt;

					q.erase(uIt);
				}

				for (auto v = adjVertices[u].begin(); v != adjVertices[u].end(); v++) {
					int alt = dist[u] + v->second;

					if (alt < dist[v->first]) {
						dist[v->first] = alt;
						prev[v->first] = u;
					}
				}
			}
		}


	private:
		std::set<Vertex *> vertexes;
		std::map<Vertex *, std::vector<std::pair<Vertex *, float>>> adjVertices;
};

#endif
