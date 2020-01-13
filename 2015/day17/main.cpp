#include <iostream>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


// Function to find all unique combination of
// given elements such that their sum is K
void unique_combination(int l, int sum, int K, std::vector<int> &local, std::vector<int> &A, std::vector<std::vector<int>> &out) {
	if (sum == K) {
		out.push_back(local);

		return;
	}

	for (int i = l; i < A.size(); i++) {
		if (sum + A[i] > K)
			continue;

		local.push_back(A[i]);

		unique_combination(i + 1, sum + A[i], K, local, A, out);

		local.pop_back();
	}
}


void combinationNum(std::vector<int> A, int K, std::vector<std::vector<int>> &out) {
	sort(A.begin(), A.end());

	std::vector<int> local;

	unique_combination(0, 0, K, local, A, out);
}


int main(int argc, char *argv[]) {
	auto data = utils::toIntV(File::readAllLines(argv[1]));

	{
		std::vector<std::vector<int>> combinations;

		combinationNum(data, utils::toInt(argv[2]), combinations);

		PRINTF(("PART_A: %zd", combinations.size()));

		{
			int min = std::numeric_limits<int>::max();
			int minCount = 0;

			for (auto &c : combinations) {
				if (c.size() < min) {
					min      = c.size();
					minCount = 1;

				} else if (c.size() == min) {
					minCount++;
				}
			}

			PRINTF(("PART_B: %d", minCount));
		}
	}
}
