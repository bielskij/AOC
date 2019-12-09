#include <sstream>

#include "utils/utils.h"


std::vector<int64_t> utils::string2Int64t(const std::vector<std::string> &src) {
	std::vector<int64_t> ret;

	for (auto it = src.begin(); it != src.end(); it++) {
		ret.push_back(std::stoll(it->c_str()));
	}

	return ret;
}


int utils::toInt(const std::string &src) {
	return atoi(src.c_str());
}


std::vector<int> utils::toIntV(const std::vector<std::string> &src) {
	std::vector<int> ret;

	for (auto it = src.begin(); it != src.end(); it++) {
		ret.push_back(toInt(it->c_str()));
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
	return std::abs((int)(l.getX() - r.getX())) + std::abs((int)(l.getY() - r.getY()));
}
