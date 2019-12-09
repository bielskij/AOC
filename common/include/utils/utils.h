#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <string>
#include <vector>

#include "common/types.h"


namespace utils {
	int toInt(const std::string &src);
	std::vector<int> toIntV(const std::vector<std::string> &src);

	std::vector<int64_t> string2Int64t(const std::vector<std::string> &src);

	std::vector<std::string> strTok(const std::string &str, char delimiter);

	int manhattanDistance(const Point &l, const Point &r);
}

#endif /* UTILS_UTILS_H_ */
