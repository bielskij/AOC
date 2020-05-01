#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/Maze.h"
#include "utils/graph.h"
#include "utils/graph/GraphSearchDijkstra.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


class Main {
	public:
		Main(int favNumber, const Point<int> &entrance, const Point<int> &exit) : _start(entrance), _end(exit) {
			this->_favNumber = favNumber;
		}

		void execute() {
			const int rowNum[] = { -1,  0, 0, 1 };
			const int colNum[] = {  0, -1, 1, 0 };

			bool visited[this->height() * this->width()]{ false };

			struct Node {
				Point<int> pos;
				int distance;

				Node(const Point<int> &p, int distance) : pos(p) {
					this->distance = distance;
				}
			};

			this->_partA = 0;
			this->_partB = 0;

			std::queue<Node> q;

			visited[this->_start.y() * this->width() + this->_start.x()] = true;

			q.push(Node(this->_start, 0));

			while (! q.empty()) {
				Node curr = q.front(); q.pop();

				if (curr.pos == this->_end) {
					this->_partA = curr.distance;
				}

				if (curr.distance <= 50) {
					this->_partB++;
				}

				for (int i = 0; i < 4; i++) {
					Point<int> next(curr.pos.x() + colNum[i], curr.pos.y() + rowNum[i]);

					if (
						pointInRange(next) &&
						! visited[next.y() * this->width() + next.x()] &&
						! isWall(next)
					) {
						visited[next.y() * this->width() + next.x()] = true;

						q.push(Node(next, curr.distance + 1));
					}
				}
			}
		}

		int getPartA() const {
			return this->_partA;
		}

		int getPartB() const {
			return this->_partB;
		}

	protected:
		int width() const {
			return this->_end.x() * 2;
		}

		int height() const {
			return this->_end.y() * 2;
		}

		bool pointInRange(const Point<int> &p) {
			if (
				(p.x() >= 0 && p.x() <= this->width()) &&
				(p.y() >= 0 && p.y() <= this->height())
			) {
				return true;
			}

			return false;
		}

		bool isWall(const Point<int> &p) const {
			int val =  p.x() * p.x() + 3 * p.x() + 2 * p.x() * p.y() + p.y() + p.y() * p.y() + this->_favNumber;

			if (getBitsCount(val) & 1) {
				return true;
			}

			return false;
		}

		static int getBitsCount(int val) {
			int c;

			for (c = 0; val; c++) {
				val &= val - 1;
			}

			return c;
		}

	private:
		int        _favNumber;
		Point<int> _start;
		Point<int> _end;

		int _partA;
		int _partB;
};


int main(int argc, char *argv[]) {
	Main m(utils::toInt(argv[1]), Point<int>(1, 1), Point<int>(utils::toInt(argv[2]), utils::toInt(argv[3])));

	m.execute();

	PRINTF(("PART_A: %d", m.getPartA()));
	PRINTF(("PART_B: %d", m.getPartB()));
}
