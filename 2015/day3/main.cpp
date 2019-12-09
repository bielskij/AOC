#include <stdexcept>
#include <climits>
#include <set>
#include <algorithm>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


int main(int argc, char *argv[]) {
	std::string data = File::readAllLines(argv[1])[0];

	{
		int xmin = INT_MAX, xmax = INT_MIN, ymin = INT_MAX, ymax = INT_MIN;

		int x = 0;
		int y = 0;

		for (auto c = data.begin(); c != data.end(); c++) {
			switch (*c) {
				case '>': x++; break;
				case '<': x--; break;
				case '^': y++; break;
				case 'v': y--; break;
				default:
					throw std::runtime_error("Unsupported code!");
			}

			if (x < xmin) xmin = x;
			if (x > xmax) xmax = x;
			if (y < ymin) ymin = y;
			if (y > ymax) ymax = y;
		}

		int xoffset = -xmin;
		int yoffset = -ymin;

		{
			int map[ymax - ymin + 1][xmax - xmin + 1];

			x = 0;
			y = 0;

			map[yoffset][xoffset] = 1;

			for (int y = 0; y < ymax - ymin + 1; y++) {
				for (int x = 0; x < xmax - xmin + 1; x++) {
					map[y][x] = 0;
				}
			}

			for (auto c = data.begin(); c != data.end(); c++) {
				switch (*c) {
					case '>': x++; break;
					case '<': x--; break;
					case '^': y++; break;
					case 'v': y--; break;
					default:
						throw std::runtime_error("Unsupported code!");
				}

				map[y + yoffset][x + xoffset]++;
			}

			int result = 0;
			for (y = 0; y < ymax - ymin + 1; y++) {
				for (x = 0; x < xmax - xmin + 1; x++) {
					if (map[y][x] >= 1) {
						result++;
					}
				}
			}

			PRINTF(("PART A: %d", result));
		}
	}

	{
		int xmin = INT_MAX, xmax = INT_MIN, ymin = INT_MAX, ymax = INT_MIN;

		int xsanta = 0;
		int xrobo  = 0;
		int ysanta = 0;
		int yrobo  = 0;

		for (int i = 0; i < data.size(); i++) {
			switch (data[i]) {
				case '>': if (i & 1) xrobo++; else xsanta++; break;
				case '<': if (i & 1) xrobo--; else xsanta--; break;
				case '^': if (i & 1) yrobo++; else ysanta++; break;
				case 'v': if (i & 1) yrobo--; else ysanta--; break;
				default:
					throw std::runtime_error("Unsupported code!");
			}

			if (i & 1) {
				if (xrobo > xmax) xmax = xrobo;
				if (xrobo < xmin) xmin = xrobo;
				if (yrobo > ymax) ymax = yrobo;
				if (yrobo < ymin) ymin = yrobo;

			} else {
				if (xsanta > xmax) xmax = xsanta;
				if (xsanta < xmin) xmin = xsanta;
				if (ysanta > ymax) ymax = ysanta;
				if (ysanta < ymin) ymin = ysanta;

			}
		}

		int xoffset = -xmin;
		int yoffset = -ymin;

		int map[ymax - ymin + 1][xmax - xmin + 1];

		xsanta = ysanta = 0;
		xrobo  = yrobo  = 0;

		map[yoffset][xoffset] = 2;

		for (int y = 0; y < ymax - ymin + 1; y++) {
			for (int x = 0; x < xmax - xmin + 1; x++) {
				map[y][x] = 0;
			}
		}

		for (int i = 0; i < data.size(); i++) {
			switch (data[i]) {
				case '>': if (i & 1) xrobo++; else xsanta++; break;
				case '<': if (i & 1) xrobo--; else xsanta--; break;
				case '^': if (i & 1) yrobo++; else ysanta++; break;
				case 'v': if (i & 1) yrobo--; else ysanta--; break;
				default:
					throw std::runtime_error("Unsupported code!");
			}

			if (i & 1) {
				map[yrobo + yoffset][xrobo + xoffset]++;

			} else {
				map[ysanta + yoffset][xsanta + xoffset]++;
			}
		}

		int result = 0;
		for (int y = 0; y < ymax - ymin + 1; y++) {
			for (int x = 0; x < xmax - xmin + 1; x++) {
				if (map[y][x] >= 1) {
					result++;
				}
			}
		}

		PRINTF(("PART B: %d", result));
	}
}
