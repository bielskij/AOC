#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/Maze.h"
#include "utils/graph.h"
#include "utils/graph/GraphSearchDijkstra.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class MazeGenerator : public Maze::Callbacks {
	private:
		static constexpr char CHAR_WALL     = '#';
		static constexpr char CHAR_PASSAGE  = '.';
		static constexpr char CHAR_ENTRANCE = 'E';
		static constexpr char CHAR_EXIT     = 'X';

	public:
		MazeGenerator(int favNumber, int exitX, int exitY) {
			this->_favNumber = favNumber;
			this->_endX      = exitX;
			this->_endY      = exitY;
			this->_width     = exitX * 2;
			this->_height    = exitY * 2;

			this->_strRepr.reserve(this->_height);

			for (int y = 0; y < this->_height; y++) {
				std::string row;

				for (int x = 0; x < this->_width; x++) {
					row.push_back(getType(x, y));
				}

				this->_strRepr.push_back(row);
			}
		}

		const std::vector<std::string> getStringReprezentation() const {
			return this->_strRepr;
		}

		void dump(FILE *out) {
			for (auto &row : this->_strRepr) {
				for (auto c : row) {
					fprintf(out, "%c", c);
				}
				fprintf(out, "\n");
			}
		}

		// Maps node type form char
		Maze::NodeType getNodeType(const Point<int> &pos, char c) const {
			switch (c) {
				case CHAR_PASSAGE:
					return Maze::NodeType::PASSAGE;

				case CHAR_WALL:
					return Maze::NodeType::WALL;

				case CHAR_ENTRANCE:
					return Maze::NodeType::ENTRANCE;

				case CHAR_EXIT:
					return Maze::NodeType::EXIT;
			}

			return Maze::NodeType::EMPTY;
		}

		// Returns identifier for logical object pair like key/door or portals
		std::string getId(const Point<int> &pos, char c, Maze::NodeType type) const {
			return "";
		}

	private:
		int getBitsCount(int val) {
			int c;

			for (c = 0; val; c++) {
				val &= val - 1;
			}

			return c;
		}

		char getType(int x, int y) {
			if (x == this->_endX && y == this->_endY) {
				return CHAR_EXIT;

			} else if (x == 1 && y == 1) {
				return CHAR_ENTRANCE;

			} else {
				int val =  x * x + 3 * x + 2 * x * y + y + y * y + this->_favNumber;

				if (getBitsCount(val) & 1) {
					return CHAR_WALL;

				} else {
					return CHAR_PASSAGE;
				}
			}
		}

	private:
		int _favNumber;
		int _width;
		int _height;
		int _endX;
		int _endY;

		std::vector<std::string> _strRepr;
};


int main(int argc, char *argv[]) {
	MazeGenerator gen(utils::toInt(argv[1]), utils::toInt(argv[2]), utils::toInt(argv[3]));

	gen.dump(stdout);

	{
		Maze maze;

		maze.parse(gen.getStringReprezentation(), gen);

		graph::Graph graph;
		maze.fillGraph(graph);

		std::vector<graph::Node *> route;

		graph::GraphSearchDijkstra dijkstra(&graph);

		dijkstra.search(maze.getEntrance()->id(), maze.getExit()->id(), route);

		int cost = 0;
		for (int i = 1; i < route.size(); i++) {
			cost += graph.getEdge(route[i - 1]->id(), route[i]->id())->cost();
		}

		PRINTF(("PART_A: %zd", cost));
	}
}
