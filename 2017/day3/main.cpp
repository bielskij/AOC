#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

int main(int argc, char *argv[]) {
	int input = utils::toInt(argv[1]);

	{
		int lastCircleLength = 0;
		int totalElements    = 1;
		int totalCircles     = 0;
		int startValue       = 2;
		int edgeLength       = 1;

		while (totalElements < input) {
			startValue       += lastCircleLength;
			lastCircleLength += 8;
			edgeLength       += 2;

			totalElements += lastCircleLength;

			totalCircles++;
		}

		{
			Point<int> startPoint(totalCircles, - (totalCircles - 1));
			Point<int> endPoint = startPoint;

			for (int i = 0; i < lastCircleLength; i++) {
				if (startValue == input) {
					break;
				}

				startValue++;

				if (i < edgeLength - 2) {
					endPoint.incY();

				} else if (i < 2 * edgeLength - 3) {
					endPoint.decX();

				} else if (i < 3 * edgeLength - 4) {
					endPoint.decY();

				} else {
					endPoint.incX();
				}
			}

			PRINTF(("PART_A: %d", utils::manhattanDistance(endPoint, Point<int>(0, 0))));
		}
	}

	{
		int edgeLength   = 3;
		int currentValue = 1;
		int loopLength   = 8;

		static const std::vector<Point<int>> adjacents = {
			Point<int>(-1 ,  1), Point<int>(0 ,  1), Point<int>(1 ,  1),
			Point<int>(-1 ,  0),                     Point<int>(1 ,  0),
			Point<int>(-1 , -1), Point<int>(0 , -1), Point<int>(1 , -1),
		};

		std::map<Point<int>, int> lastLoop;
		Point<int> p(0, 0);

		lastLoop[p] = 1;

		p.incX();

		while (currentValue <= input) {
			std::map<Point<int>, int> currentLoop;

			if (currentValue <= input) {
				for (int i = 0; i < loopLength; i++) {
					int elementValue = 0;

					for (const auto &a : adjacents) {
						Point<int> tmp = p + a;

						auto it = lastLoop.find(tmp);
						if (it != lastLoop.end()) {
							elementValue += it->second;

						} else {
							auto it = currentLoop.find(tmp);
							if (it != currentLoop.end()) {
								elementValue += it->second;
							}
						}
					}

					currentValue = elementValue;
					currentLoop[p] = currentValue;

					if (currentValue > input) {
						break;
					}

					if (i < edgeLength - 2) {
						p.incY();

					} else if (i < 2 * edgeLength - 3) {
						p.decX();

					} else if (i < 3 * edgeLength - 4) {
						p.decY();

					} else {
						p.incX();
					}
				}
			}

			loopLength += 8;

			if (currentValue > input) {
				PRINTF(("PART_B: %d", currentValue));
				break;
			}

			if (! currentLoop.empty()) {
				lastLoop = currentLoop;
			}

			edgeLength += 2;
		}
	}

	return 0;
}
