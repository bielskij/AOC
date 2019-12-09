#include <algorithm>
#include <limits.h>

#include "common/types.h"

#include "utils/utils.h"
#include "utils/file.h"

#define DEBUG_LEVEL 4
#include "common/debug.h"


struct Line {
	Point start;
	Point end;

	Line(const Point &start, const Point &end) {
		this->start = start;
		this->end   = end;
	}
};


struct Wire {
	std::vector<Line> lines;
};


static bool intersection(const Line &l1, const Line &l2, Point &intersection) {
	float uA = ((l2.end.getX() - l2.start.getX()) * (l1.start.getY() - l2.start.getY()) - (l2.end.getY() - l2.start.getY()) * (l1.start.getX() - l2.start.getX())) / ((l2.end.getY() - l2.start.getY()) * (l1.end.getX() - l1.start.getX()) - (l2.end.getX() - l2.start.getX()) * (l1.end.getY() - l1.start.getY()));
	float uB = ((l1.end.getX() - l1.start.getX()) * (l1.start.getY() - l2.start.getY()) - (l1.end.getY() - l1.start.getY()) * (l1.start.getX() - l2.start.getX())) / ((l2.end.getY() - l2.start.getY()) * (l1.end.getX() - l1.start.getX()) - (l2.end.getX() - l2.start.getX()) * (l1.end.getY() - l1.start.getY()));

	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
		intersection.setX(l1.start.getX() + (uA * (l1.end.getX() - l1.start.getX())));
		intersection.setY(l1.start.getY() + (uA * (l1.end.getY() - l1.start.getY())));

		return true;
	}

	return false;
}


static bool pointOnLine(const Line &line, const Point &point) {
	float dxc = point.getX() - line.start.getX();
	float dyc = point.getY() - line.start.getY();
	float dx1 = line.end.getX() - line.start.getX();
	float dy1 = line.end.getY() - line.start.getY();

	return (dxc * dy1 - dyc * dx1) == 0 ? true : false;
}


int main(int argc, char *argv[]) {
	std::vector<Wire> wires;

	std::vector<std::string> _data = File::readAllLines(argv[1]);

	for (auto wireIt = _data.begin(); wireIt != _data.end(); wireIt++) {
		Wire  w;

		{
			std::vector<std::string> wireRules = utils::strTok(*wireIt, ',');

			Point start;
			Point end;

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

				w.lines.push_back(Line(start, end));
			}
		}

		wires.push_back(w);
	}

	// Step A
	{
		std::vector<Point> intersections;

		for (auto wireChecked = wires.begin(); wireChecked != wires.end(); wireChecked++) {
			for (auto otherWire = wires.begin(); otherWire != wires.end(); otherWire++) {
				// Skip the same wire
				if (wireChecked != otherWire) {
					// Each line in wireChecked
					for (auto lineChecked = wireChecked->lines.begin(); lineChecked != wireChecked->lines.end(); lineChecked++) {
						for (auto lineOther = otherWire->lines.begin(); lineOther != otherWire->lines.end(); lineOther++) {
							Point intersectionPoint;

							if (intersection(*lineChecked, *lineOther, intersectionPoint)) {
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
			int distance = utils::manhattanDistance(*p, Point(0, 0));

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

					if (pointOnLine(*line, *intersection)) {
						intersectionDistance = utils::manhattanDistance(line->start, *intersection);
					}

					if (intersectionDistance < 0) {
						pathLength += utils::manhattanDistance(line->start, line->end);

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
