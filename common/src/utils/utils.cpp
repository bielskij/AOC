#include <sstream>

#include "utils/utils.h"


int64_t utils::toInt64t(const std::string &src) {
	return std::stoll(src.c_str());
}


std::vector<int64_t> utils::toInt64tV(const std::vector<std::string> &src) {
	std::vector<int64_t> ret;

	for (auto it = src.begin(); it != src.end(); it++) {
		ret.push_back(toInt64t(*it));
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


int utils::countSubstring(const std::string& str, const std::string& sub) {
	if (sub.length() == 0) {
		return 0;
	}

	int count = 0;

	for (size_t offset = str.find(sub); offset != std::string::npos; offset = str.find(sub, offset + sub.length())) {
		++count;
	}

	return count;
}
