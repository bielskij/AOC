#include <sstream>

#include "utils/utils.h"


std::vector<int> utils::string2Int(const std::vector<std::string> &src) {
	std::vector<int> ret;

	for (auto it = src.begin(); it != src.end(); it++) {
		ret.push_back(atoi(it->c_str()));
	}

	return ret;
}


std::vector<std::string> utils::strTok(const std::string &str, char delimiter) {
	std::vector<std::string> ret;

	{
		std::stringstream strStream(str);

		while (strStream.good()) {
			std::string substr;

			std::getline(strStream, substr, delimiter);

			ret.push_back(substr);
		}
	}

	return ret;
}


int utils::manhattanDistance(const Point &l, const Point &r) {
	return std::abs(l.getX() - r.getX()) + std::abs(l.getY() - r.getY());
}
