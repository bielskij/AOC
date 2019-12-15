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


std::string utils::toString(int value) {
	return utils::toString((int64_t)value);
}


std::string utils::toString(int64_t value) {
	std::stringstream ss;

	ss << value;

	return ss.str();
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


bool utils::isNumber(const std::string& s) {
	std::string::const_iterator it = s.begin();

	while (it != s.end() && std::isdigit(*it)) ++it;

	return !s.empty() && it == s.end();
}


std::string utils::ltrim(const std::string &src, const std::string &chars) {
	size_t start = src.find_first_not_of(chars);

	return (start == std::string::npos) ? "" : src.substr(start);
}


std::string utils::rtrim(const std::string &src, const std::string &chars) {
	size_t end = src.find_last_not_of(chars);

	return (end == std::string::npos) ? "" : src.substr(0, end + 1);
}


std::string utils::trim(const std::string &src, const std::string &chars) {
	return rtrim(ltrim(src, chars), chars);
}
