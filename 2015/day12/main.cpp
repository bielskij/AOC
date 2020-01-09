#include <jsoncpp/json/json.h>
#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"
#include "utils/graph.h"


#define DEBUG_LEVEL 5
#include "common/debug.h"


void visit(Json::Value &value, int &sumA, int &sumB, bool ignore) {
	if (value.isArray()) {
		for (auto item : value) {
			visit(item, sumA, sumB, ignore);
		}

	} else if (value.isObject()) {
		if (! ignore) {
			for (auto attr : value.getMemberNames()) {
				Json::Value &val = value[attr];

				if (val.isString()) {
					if (val.asString().compare("red") == 0) {
						ignore = true;
					}
				}
			}
		}

		for (auto attr : value.getMemberNames()) {
			visit(value[attr], sumA, sumB, ignore);
		}

	} else if (value.isNumeric()) {
		sumA += value.asInt();

		if (! ignore) {
			sumB += value.asInt();
		}
	}
}


int main(int argc, char *argv[]) {
	std::string data = File::readAllLines(argv[1])[0];

	Json::Value root;

	{
		Json::Reader reader;

		reader.parse(data, root, false);
	}

	int sumA = 0;
	int sumB = 0;

	visit(root, sumA, sumB, false);

	PRINTF(("PART_A: %d", sumA));
	PRINTF(("PART_B: %d", sumB));
}
