#include <algorithm>
#include <limits.h>

#include "common/types.h"

#include "utils/utils.h"
#include "utils/file.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"




struct Wire {
	std::vector<Line<float>> lines;
};


int main(int argc, char *argv[]) {
	std::vector<Wire> wires;

	std::vector<std::string> _data = File::readAllLines(argv[1]);

	for (auto wireIt = _data.begin(); wireIt != _data.end(); wireIt++) {
		Wire  w;

		{
			std::vector<std::string> wireRules = utils::strTok(*wireIt, ',');

			Point<float> start;
			Point<float> end;

			for (auto ruleIt = wireRules.begin(); ruleIt != wireRules.end(); ruleIt++) {
				int length = atoi(ruleIt->c_str() + 1);

				start = end;

				switch (ruleIt->data()[0]) {
					case 'R':
						end.setX(end.getX() + length);
						break;

					case 'L':
						end.setX(end.getX() - length);
						break;

					case 'U':
						end.setY(end.getY() + length);
						break;

					case 'D':
						end.setY(end.getY() - length);
						break;
				}

				w.lines.push_back(Line<float>(start, end));
			}
		}

		wires.push_back(w);
	}

	// Step A
	{
		std::vector<Point<float>> intersections;

		for (auto wireChecked = wires.begin(); wireChecked != wires.end(); wireChecked++) {
			for (auto otherWire = wires.begin(); otherWire != wires.end(); otherWire++) {
				// Skip the same wire
				if (wireChecked != otherWire) {
					// Each line in wireChecked
					for (auto lineChecked = wireChecked->lines.begin(); lineChecked != wireChecked->lines.end(); lineChecked++) {
						for (auto lineOther = otherWire->lines.begin(); lineOther != otherWire->lines.end(); lineOther++) {
							Point<float> intersectionPoint;

							if (lineChecked->crossTrough(*lineOther, intersectionPoint)) {
								if (intersectionPoint.getX() != 0 && intersectionPoint.getY() != 0) {
									if (std::find(intersections.begin(), intersections.end(), intersectionPoint) == intersections.end()) {
										intersections.push_back(intersectionPoint);
									}
								}
							}
						}
					}
				}
			}
		}

		int minDistance = INT_MAX;

		for (auto p = intersections.begin(); p != intersections.end(); p++) {
			int distance = utils::manhattanDistance<float>(*p, Point<float>(0, 0));

			if (distance < minDistance) {
				minDistance = distance;
			}
		}

		PRINTF(("Step 1: %d", minDistance));

		// Step B

		int shortestDistance = INT_MAX;

		for (auto intersection = intersections.begin(); intersection != intersections.end(); intersection++) {
			std::vector<int> distances;

			for (auto wire = wires.begin(); wire != wires.end(); wire++) {
				int pathLength = 0;

				for (auto line = wire->lines.begin(); line != wire->lines.end(); line++) {
					int intersectionDistance = -1;

					if (line->crossTrough(*intersection)) {
						intersectionDistance = utils::manhattanDistance(line->getStart(), *intersection);
					}

					if (intersectionDistance < 0) {
						pathLength += utils::manhattanDistance(line->getStart(), line->getEnd());

					} else {
						pathLength += intersectionDistance;

						break;
					}
				}

				distances.push_back(pathLength);
			}

			{
				int sum = 0;

				for (auto d = distances.begin(); d != distances.end(); d++) {
					sum += *d;
				}

				if (sum < shortestDistance) {
					shortestDistance = sum;
				}
			}
		}

		PRINTF(("Step 2: %d", shortestDistance));
	}
}
