#include <cassert>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static int distance(const Point<int> &x, const Point<int> &y) {
	int distX = std::abs(x.x() - y.x());
	int distY = std::abs(x.y() - y.y());

	return std::max(distX, distY);
}


static int solve(const std::vector<std::string> &lines, int length) {
	std::vector<Point<int>> rope;

	for (int i = 0; i < length; i++) {
		rope.push_back(Point<int>(Point<int>(0, 0)));
	}

	std::set<Point<int>> tailUniquePoints;

	for (const auto &l : lines) {
		auto toks = utils::strTok(l, ' ');

		assert(toks.size() == 2);

		int modX = 0;
		int modY = 0;

		switch (toks[0][0]) {
			case 'U': modY =  1; break;
			case 'D': modY = -1; break;
			case 'L': modX = -1; break;
			case 'R': modX =  1; break;

			default: assert(false);
		}

		for (int i = 0; i < utils::toInt(toks[1]); i++) {
			// Advance head
			{
				auto &head = rope[0];

				head.x(head.x() + modX);
				head.y(head.y() + modY);
			}

			// Advance segments
			for (int segment = 1; segment < rope.size(); segment++) {
				auto &h = rope[segment - 1];
				auto &t = rope[segment];

				if (distance(h, t) <= 1) {
					break;

				} else {
					/*
					 * ..H.H.H..
					 * ..HXXXH..
					 * ..HXTXH..
					 * ..HXXXH..
					 * ..H.H.H..
					 */
					if (t.y() > h.y() + 1) {
						t.decY();
						if (t.x() > h.x()) {
							t.decX();
						} else if (t.x() < h.x()) {
							t.incX();
						}

					} else if (t.y() < h.y() -1) {
						t.incY();
						if (t.x() > h.x()) {
							t.decX();
						} else if (t.x() < h.x()) {
							t.incX();
						}

					} else if (t.x() > h.x() + 1) {
						t.decX();
						if (t.y() > h.y()) {
							t.decY();
						} else if (t.y() < h.y()) {
							t.incY();
						}

					} else if (t.x() < h.x() - 1) {
						t.incX();
						if (t.y() > h.y()) {
							t.decY();
						} else if (t.y() < h.y()) {
							t.incY();
						}
					}
				}
			}

			tailUniquePoints.insert(*rope.rbegin());
		}
	}

	return tailUniquePoints.size();
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	PRINTF(("PART_A: %d", solve(lines, 2)));
	PRINTF(("PART_B: %d", solve(lines, 10)));

	return 0;
}
