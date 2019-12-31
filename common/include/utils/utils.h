#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

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

	void genCombinationsUtil(std::vector<std::vector<int> > &combinations, std::vector<int> &tmp, int n, int left, int k);
	// Generates combinations from elements between <1 and n>
	void genCombinations(std::vector<std::vector<int>> &out, int n, int k);

	template <class T>
	void genPermutation(std::vector<std::vector<T>> &out, std::vector<T> &in) {
		int c[in.size()] = { 0 };

		out.push_back(in);

		int i = 0;
		while (i < in.size()) {
			if (c[i] < i) {
				if (i % 2 == 1) {
					std::swap(in[c[i]], in[i]);
				} else {
					std::swap(in[0], in[i]);
				}

				out.push_back(in);

				c[i]++;
				i = 0;

			} else {
				c[i] = 0;

				i++;
			}
		}
	}

	uint64_t llrand(uint64_t max);
}

#endif /* UTILS_UTILS_H_ */
