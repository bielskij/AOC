#include <iostream>
#include <cstring>
#include <climits>
#include <algorithm>

#include "utils/utils.h"
#include "utils/file.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


static void _getFFT(int *dataIn, int *dataOut, int dataSize, int phases) {
	int tmp[2][dataSize];
	int tmpSrcIdx = 0;
	int tmpDstIdx = 1;

	memcpy(tmp[tmpSrcIdx], dataIn, dataSize * sizeof(*dataIn));

	for (int phase = 0; phase < phases; phase++) {
		for (int num = 0; num < dataSize; num++) {
			int *src = tmp[tmpSrcIdx];
			int *dst = tmp[tmpDstIdx];

			int sum = 0;

			for (int i = num; i < dataSize; i += ((num + 1) * 2)) {
				for (int j = 0; j < num + 1; j++) {
					if (i + j < dataSize) {
						switch (((i + 1 + j) / (num + 1)) & 0x03) {
							case 1: sum += src[i + j]; break;
							case 3: sum -= src[i + j]; break;
						}

					} else {
						break;
					}
				}
			}

			dst[num] = std::abs(sum) % 10;
		}

		std::swap(tmpSrcIdx, tmpDstIdx);
	}

	memcpy(dataOut, tmp[tmpSrcIdx], dataSize * sizeof(*dataOut));
}


int main(int argc, char *argv[]) {
	std::vector<int> data;

	{
		std::string dataStr = File::readAllLines(argv[1])[0];

		for (auto c = dataStr.begin(); c != dataStr.end(); c++) {
			data.push_back(*c - 48);
		}
	}

	{
		int in[data.size()];
		int out[data.size()];

		for (int i = 0; i < data.size(); i++) {
			in[i] = data[i];
		}

		_getFFT(in, out, data.size(), utils::toInt(argv[2]));

		PRINTF(("PART_A: %d%d%d%d%d%d%d%d", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7]));
	}

	// Second half of the phase matrix is a triangle of '1' and '0' only
	{
		int blockSize = data.size();
		int totalSize = blockSize * 10000;
		int dataSize  = 0;
		int offset    = 0;

		for (int i = 0; i < 7; i++) {
			offset *= 10;
			offset += data[i];
		}

		dataSize = totalSize - offset;

		int srcData[dataSize];
		int dstData[dataSize];

		// Only data since offset position
		for (int i = 0; i < dataSize; i++) {
			srcData[i] = data[(offset + i) % data.size()];
		}

		for (int loop = 0; loop < 100; loop++) {
			int sum = 0;

			for (int i = dataSize - 1; i >= 0; i--) {
				sum += srcData[i];

				dstData[i] = sum % 10;
			}

			memcpy(srcData, dstData, sizeof(int) * dataSize);
		}

		PRINTF(("PART_B: %d%d%d%d%d%d%d%d", dstData[0], dstData[1], dstData[2], dstData[3], dstData[4], dstData[5], dstData[6], dstData[7]));
	}
}
