#include <iostream>
#include <cstring>
#include <algorithm>
#include <queue>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/IntCodeMachine.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


#define TO_INT(c) ((c) - 48)

int main(int argc, char *argv[]) {
	std::string data = File::readAllLines(argv[1])[0];

	const int wide = 25;
	const int tall = 6;

	const int pixelsOnLayers = wide * tall;

	std::vector<std::string> layers;

	for (int i = 0; i < data.size(); i += pixelsOnLayers) {
		layers.push_back(data.substr(i, pixelsOnLayers));
	}

	{
		int minIndex = 0;
		int minValue = std::count(layers[0].begin(), layers[0].end(), '0');;

		for (int i = 1; i < layers.size(); i++) {
			int layerValue = std::count(layers[i].begin(), layers[i].end(), '0');

			if (layerValue < minValue) {
				minValue = layerValue;
				minIndex = i;
			}
		}

		PRINTF(("PART A: %zd",
			std::count(layers[minIndex].begin(), layers[minIndex].end(), '1') *
			std::count(layers[minIndex].begin(), layers[minIndex].end(), '2')
		));
	}

	{
		std::string finalImage;

		for (int pixel = 0; pixel < pixelsOnLayers; pixel++) {
			char color = '2';

			for (int layer = 0; layer < layers.size(); layer++) {
				color = layers[layer][pixel];
				if (color == '1' || color == '0') {
					break;
				}
			}

			finalImage += color;
		}

		PRINTF(("PART B"));

		for (int y = 0; y < tall; y++) {
			for (int x = 0; x < wide; x++) {
				int idx = y * wide + x;

				printf("%c", finalImage[idx] == '1' ? '#' : ' ');
			}

			printf("\n");
		}
	}
}
