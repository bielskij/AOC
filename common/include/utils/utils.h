#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <string>
#include <vector>

#include "common/types.h"


namespace utils {
	std::vector<int> string2Int(const std::vector<std::string> &src);

	std::vector<long long int> string2LongLongInt(const std::vector<std::string> &src);

	std::vector<std::string> strTok(const std::string &str, char delimiter);

	int manhattanDistance(const Point &l, const Point &r);
}

#endif /* UTILS_UTILS_H_ */
