#include <cmath>
#include <climits>

#include <string>
#include <vector>
#include <algorithm>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

static Vector2D startVector(-4, 0);


struct VectorComparator {
	bool operator()(const Vector2D &a, const Vector2D &b) const {
		float angleA = a.getAngle(startVector);
		float angleB = b.getAngle(startVector);

		if (angleA < 0) {
			angleA += 360;
		}

		if (angleB < 0) {
			angleB += 360;
		}

		if (angleA < angleB) {
			return true;

		} else if (angleA == angleB) {
			return a.getLength() < b.getLength();
		}

		return false;
	}
};


int main(int argc, char *argv[]) {
	std::vector<Point<int>> asteroids;
	int width = 0;
	int height = 0;

	{
		auto data = File::readAllLines(argv[1]);

		for (int row = 0; row < data.size(); row++) {
			std::string &line = data[row];

			for (int col = 0; col < line.size(); col++) {
				if (line[col] == '#') {
					asteroids.push_back(Point<int>(col, row));
				}
			}
		}
	}

	DBG(("Have %zd asteroids", asteroids.size()));

	Point<int> location;

	{
		int   asteroidNumber = 0;

		for (auto src = asteroids.begin(); src != asteroids.end(); src++) {
			int asteroidsInSight = 0;

			for (auto dst = asteroids.begin(); dst != asteroids.end(); dst++) {
				if (*src != *dst) {
					Line<int> l(*src, *dst);

					int minX = src->getX() < dst->getX() ? src->getX() : dst->getX();
					int minY = src->getY() < dst->getY() ? src->getY() : dst->getY();
					int maxX = src->getX() > dst->getX() ? src->getX() : dst->getX();
					int maxY = src->getY() > dst->getY() ? src->getY() : dst->getY();

					bool hasCollision = false;

					for (int y = minY; y <= maxY; y++) {
						for (int x = minX; x <= maxX; x++) {
							Point<int> p(x, y);

							if (p != *src && p != *dst) {
								if (l.crossTrough(p)) {
									for (auto it = asteroids.begin(); it != asteroids.end(); it++) {
										if (*it == p) {
											hasCollision = true;
											break;
										}
									}
								}
							}
						}

						if (hasCollision) {
							break;
						}
					}

					if (! hasCollision) {
						asteroidsInSight++;
					}
				}
			}

			if (asteroidsInSight > asteroidNumber) {
				asteroidNumber = asteroidsInSight;
				location = *src;
			}
		}

		PRINTF(("PART A: %d (%d, %d)", asteroidNumber, (int)location.getX(), (int)location.getY()));
	}

	{
		std::vector<Vector2D> vectors;

		for (auto asteroid = asteroids.begin(); asteroid != asteroids.end(); asteroid++) {
			if (*asteroid != location) {
				vectors.push_back(Vector2D(asteroid->getY() - location.getY(), asteroid->getX() - location.getX()));
			}
		}

		std::sort(vectors.begin(), vectors.end(), VectorComparator());

		int idx = 1;
		while (! vectors.empty()) {
			auto vector = vectors.begin();
			float lastAngle = 400;

			while (vector != vectors.end()) {
				float angle = vector->getAngle(startVector);

				if (lastAngle == angle) {
					vector++;
					continue;
				}

				if (idx == 200) {
					int x = (int)(vector->getY() + location.getX());
					int y = (int)(vector->getX() + location.getY());

					vectors.clear();
					PRINTF(("PART B: %d", x * 100 + y));
					break;

				} else {
					vector = vectors.erase(vector);

					idx++;
					lastAngle = angle;
				}
			}
		}
	}
}
