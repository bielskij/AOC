#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <string>
#include <vector>

#include "common/types.h"


namespace utils {
	int toInt(const std::string &src);
	std::vector<int> toIntV(const std::vector<std::string> &src);

	int64_t toInt64t(const std::string &src);
	std::vector<int64_t> toInt64tV(const std::vector<std::string> &src);

	std::vector<std::string> strTok(const std::string &str, char delimiter);

	int manhattanDistance(const Point &l, const Point &r);

	int countSubstring(const std::string& str, const std::string& sub);
}

#endif /* UTILS_UTILS_H_ */
