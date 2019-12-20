#ifndef UTILS_GRAPH_GRAPHSEARCH_H_
#define UTILS_GRAPH_GRAPHSEARCH_H_

#include <vector>

#include "Node.h"
#include "Edge.h"

namespace graph {
	class GraphSearch {
		public:
			virtual ~GraphSearch() {
			}

			virtual void search(int startID, int endID, std::vector<Node *> &route, bool rememberExaminedEdges = false) = 0;

			virtual void getExaminedEdges(std::vector<Edge *> &edges) = 0;

			virtual void getRoute(std::vector<Node *> &route) = 0;
	};
}


#endif /* UTILS_GRAPH_GRAPHSEARCH_H_ */
