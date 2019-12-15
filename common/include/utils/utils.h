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

	std::string toString(int value);
	std::string toString(int64_t value);

	template <class T>
	T manhattanDistance(const Point<T> &l, const Point<T> &r) {
		return std::abs(l.getX() - r.getX()) + std::abs(l.getY() - r.getY());
	}

	int countSubstring(const std::string& str, const std::string& sub);

	bool isNumber(const std::string& s);

	std::string ltrim(const std::string &src, const std::string &chars);
	std::string rtrim(const std::string &src, const std::string &chars);
	std::string trim(const std::string &src, const std::string &chars);

	template <class T>
	T nwd(T a, T b) {
		while(a != b)
			if(a > b)
				a -= b;
			else
				b -= a;

		return a;
	}

	template <class T>
	T nww(T a, T b) {
		return (a * b) / nwd(a, b);
	}
}

#endif /* UTILS_UTILS_H_ */
