#ifndef UTILS_GRAPH_NODE_H_
#define UTILS_GRAPH_NODE_H_

namespace graph {
	class Node  {
		protected:
			Node(){
				this->_id = -1;
			}

		public:
			Node(int id){
				this->_id = id;
			}

			int id() const {
				return this->_id;
			}

			void id(int id) {
				this->_id = id;
			}

		protected:
			int _id;
	};
}

#endif /* UTILS_GRAPH_NODE_H_ */
