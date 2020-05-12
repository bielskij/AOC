#include <stack>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

// #########
// #S| | | #
// #-#-#-#-#
// # | | | #
// #-#-#-#-#
// # | | | #
// #-#-#-#-#
// # | | |
// ####### V

class Main {
	public:
		Main(const std::string &passcode, const Point<int> &entrance, const Point<int> &exit) : _passcode(passcode), _start(entrance), _end(exit) {
			this->_partB = 0;
		}

		bool isDoor(const Point<int> &p) {
			if (p.x() == 0 || p.x() == this->_end.x()) {
				return false;
			}

			if (p.y() == 0 || p.y() == this->_end.y()) {
				return false;
			}

			// 2, 1
			// 1, 2
			if (
				((p.x() % 2) == 0 && (p.y() % 2 == 1)) ||
				((p.x() % 2) == 1 && (p.y() % 2 == 0))
			) {
				return true;
			}

			return false;
		}

		static bool isDoorOpen(char c) {
			return c >= 'b' && c <= 'f';
		}

		void execute() {
			const int rowNum[] = {  -1, 1,  0, 0 }; // y
			const int colNum[] = {  0,  0, -1, 1 }; // x

			bool visited[this->height() * this->width()]{ false };

			struct Node {
				Point<int> pos;
				int distance;
				std::string path;

				Node(const Point<int> &p, int distance, const std::string &path) : pos(p), path(path) {
					this->distance  = distance;
				}
			};

			std::stack<Node> q;

			visited[this->_start.y() * this->width() + this->_start.x()] = true;

			q.push(Node(this->_start, 0, this->_passcode));

			while (! q.empty()) {
				Node curr = q.top(); q.pop();

				if (
					(curr.pos.x() == this->_end.x() - 1) &&
					(curr.pos.y() == this->_end.y() - 1)
				) {
					if (this->_partA.empty()) {
						this->_partA = curr.path;

					} else if (this->_partA.length() > curr.path.length()) {
						this->_partA = curr.path;
					}

					if (curr.distance > this->_partB) {
						this->_partB = curr.distance;
					}

					continue;
				}

				std::string hash = utils::md5(curr.path, true);

				for (int i = 0; i < 4; i++) {
					Point<int> nextCross(curr.pos.x() + colNum[i], curr.pos.y() + rowNum[i]);
					Point<int> nextDest(nextCross.x() + colNum[i], nextCross.y() + rowNum[i]);

					if (pointInRange(nextCross) && pointInRange(nextCross)) {
						if (! isWall(nextCross)) {
							if (isDoorOpen(hash[i])) {
								// Go trough the door
								visited[nextDest.y() * this->width() + nextDest.x()] = true;

								switch (i) {
									case 0: q.push(Node(nextDest, curr.distance + 1, curr.path + "U")); break;
									case 1: q.push(Node(nextDest, curr.distance + 1, curr.path + "D")); break;
									case 2: q.push(Node(nextDest, curr.distance + 1, curr.path + "L")); break;
									case 3: q.push(Node(nextDest, curr.distance + 1, curr.path + "R")); break;
								}
							}
						}
					}
				}
			}
		}

		std::string getPartA() const {
			return this->_partA;
		}

		int getPartB() const {
			return this->_partB;
		}

	protected:
		int width() const {
			return this->_end.x() + 1;
		}

		int height() const {
			return this->_end.y() + 1;
		}

		bool pointInRange(const Point<int> &p) {
			if (
				(p.x() >= 0 && p.x() < this->width()) &&
				(p.y() >= 0 && p.y() < this->height())
			) {
				return true;
			}

			return false;
		}

		bool isWall(const Point<int> &p) const {
			if (
				(p == this->_end) ||
				((p.x() == this->_end.x() - 1) && (p.y() == this->_end.y())) ||
				((p.x() == this->_end.x()    ) && (p.y() == this->_end.y() - 1))
			) {
				return false;
			}

			if (p.x() == 0 || p.x() == this->_end.x()) {
				return true;
			}

			if (p.y() == 0 || p.y() == this->_end.y()) {
				return true;
			}

			if (((p.x() % 2) == 0) && ((p.y() % 2) == 0)) {
				return true;
			}

			return false;
		}

	private:
		std::string _passcode;
		Point<int> _start;
		Point<int> _end;

		std::string _partA;
		int         _partB;
};

int main(int argc, char *argv[]) {
	Main parta(argv[1], Point<int>(1, 1), Point<int>(8, 8));

	parta.execute();

	PRINTF(("PART_A: %s", parta.getPartA().c_str()));
	PRINTF(("PART_B: %d", parta.getPartB()));
}
