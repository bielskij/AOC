#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		Point<int> position;
		int angle = 90;

		for (auto &l : lines) {
			int distance = utils::toInt(l.substr(1));
			int turnAngle = -1;

			switch (l[0]) {
				case 'N': turnAngle =   0; break;
				case 'E': turnAngle =  90; break;
				case 'S': turnAngle = 180; break;
				case 'W': turnAngle = 270; break;

				case 'F':
					break;

				case 'L':
					{
						angle = (angle + 360 - distance) % 360;

						distance = 0;
					}
					break;

				case 'R':
					{
						angle = (angle + distance) % 360;

						distance = 0;
					}
					break;

				default:
					break;
			}

			{
				int modX = 0;
				int modY = 0;

				switch (turnAngle >= 0 ? turnAngle : angle) {
					case 0:   modY = 1;  break;
					case 90:  modX = 1;  break;
					case 180: modY = -1; break;
					case 270: modX = -1; break;
					default:
						break;
				}

				position.x(position.x() + modX * distance);
				position.y(position.y() + modY * distance);
			}

		}
		PRINTF(("PART_A: %d", std::abs(position.x()) + std::abs(position.y())));
	}

	{
		Point<int> shipPosition;
		Point<int> waypoint(10, 1);

		for (auto &l : lines) {
			int distance = utils::toInt(l.substr(1));

			switch (l[0]) {
				case 'N': waypoint.y(waypoint.y() + distance); break;
				case 'E': waypoint.x(waypoint.x() + distance); break;
				case 'S': waypoint.y(waypoint.y() - distance); break;
				case 'W': waypoint.x(waypoint.x() - distance); break;

				case 'F':
					{
						shipPosition.x(shipPosition.x() + waypoint.x() * distance);
						shipPosition.y(shipPosition.y() + waypoint.y() * distance);
					}
					break;

				case 'R':
					{
						int num = distance / 90;

						while (num--) {
							int tmp = waypoint.x();
							waypoint.x(waypoint.y());
							waypoint.y(-tmp);
						}
					}
					break;

				case 'L':
					{
						int num = distance / 90;

						while (num--) {
							int tmp = waypoint.x();
							waypoint.x(-waypoint.y());
							waypoint.y(tmp);
						}
					}
					break;
			}
		}
		PRINTF(("PART_B: %d", std::abs(shipPosition.x()) + std::abs(shipPosition.y())));
	}

	return 0;
}
