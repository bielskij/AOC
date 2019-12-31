#ifndef UTILS_GRAPH_EDGE_H_
#define UTILS_GRAPH_EDGE_H_

#include <cmath>

#include "Node.h"

namespace graph {
	class Edge {
		public:
			Edge(){
				this->_cost = 1.0;

				this->_from = this->_to = nullptr;
			}

			Edge(Node *from, Node *to) {
				this->_from = from;
				this->_to   = to;
			}

			Edge(const Edge &edge){
				this->_from = edge._from;
				this->_to   = edge._to;
				this->_cost = edge._cost;
			}

			Edge(const Edge &edge, double costSoFar) {
				this->_from = edge._from;
				this->_to   = edge._to;
				this->_cost = costSoFar;
			}

			Edge(Node *from, Node *to, double cost) {
				this->_from = from;
				this->_to   = to;
				this->_cost = cost;
			}

			virtual ~Edge() {
			}

			double cost() const {
				return this->_cost;
			}

			void cost(double cost) {
				this->_cost = cost;
			}

			Node *from() {
				return _from;
			}

			Node *to() {
				return _to;
			}

		protected:
			Node  *_from;
			Node  *_to;
			double _cost = 1.0;
	};
}

#endif /* UTILS_GRAPH_EDGE_H_ */
