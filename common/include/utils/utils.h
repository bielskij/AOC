#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <cstdint>

#include "common/types.h"


namespace utils {
	int toInt(const std::string &src);

	template <typename intType>
	std::vector<intType> toIntV(const std::vector<std::string> &src) {
		std::vector<intType> ret;

		for (auto it = src.begin(); it != src.end(); it++) {
			ret.push_back(std::stoll(it->c_str()));
		}

		return ret;
	}

	int64_t toInt64t(const std::string &src);
	std::vector<int64_t> toInt64tV(const std::vector<std::string> &src);

	// JAVA Math.floorDiv
	template<typename type>
	static type floorDiv(type x, type y) {
		type r = x / y;
		// if the signs are different and modulo not zero, round down
		if ((x ^ y) < 0 && (r * y != x)) {
			r--;
		}

		return r;
	}

	// JAVA Math.floorMod
	template<typename type>
	static type floorMod(type x, type y) {
		type r = x - floorDiv<type>(x, y) * y;
		return r;
	}

	std::vector<std::string> strTok(const std::string &str, char delimiter);

	std::string toString(int value);
	std::string toString(int64_t value);

	template <class T>
	T manhattanDistance(const Point<T> &l, const Point<T> &r) {
		return std::abs(l.x() - r.x()) + std::abs(l.y() - r.y());
	}

	template <class T>
	T manhattanDistance(const Point3d<T> &l, const Point3d<T> &r) {
		return std::abs(l.x() - r.x()) + std::abs(l.y() - r.y()) + std::abs(l.z() - r.z());
	}

	int countSubstring(const std::string& str, const std::string& sub);

	bool isNumber(const std::string& s);
	bool isHex(const std::string &s);

	std::string ltrim(const std::string &src, const std::string &chars);
	std::string rtrim(const std::string &src, const std::string &chars);
	std::string trim(const std::string &src, const std::string &chars);

	std::string string(const char *format, ...);

	template <class T>
	T gcd(T a, T b) {
		while(a != b)
			if(a > b)
				a -= b;
			else
				b -= a;

		return a;
	}

	template <class T>
	T gcd_ext(T a, T b, T &x, T &y) {
		if (a == 0) {
			x = 0;
			y = 1;

			return b;
		}

		T x1;
		T y1;

		T gcdVal = gcd_ext(b % a, a, x1, y1);

		x = y1 - (b / a) * x1;
		y = x1;

		return gcdVal;
	}

	template <class T>
	T nww(T a, T b) {
		return (a * b) / gcd(a, b);
	}

	template <class T>
	T nww(const std::vector<T> &factors) {
		T ret = factors[0];

		for (size_t i = 1; i < factors.size(); i++) {
			ret = nww<T>(ret, factors[i]);
		}

		return ret;
	}

	void genCombinationsUtil(std::vector<std::vector<int> > &combinations, std::vector<int> &tmp, int n, int left, int k);
	// Generates combinations from elements between <1 and n>
	void genCombinations(std::vector<std::vector<int>> &out, int n, int k);

	template <class T>
	void genPermutation(std::vector<std::vector<T>> &out, std::vector<T> &in) {
		int c[in.size()] = { 0 };

		out.clear();
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

	template <class T>
	void genKCombinationUtil(const std::vector<T> &arr, std::vector<std::vector<T>> &out, T *tmp, int start, int end, int index, int r) {
		if (index == r) {
			std::vector<T> comb;

			for (int j = 0; j < r; j++)
				comb.push_back(tmp[j]);

			out.push_back(comb);

			return;
		}

		for (int i = start; i <= end && end - i + 1 >= r - index; i++) {
			tmp[index] = arr[i];
			genKCombinationUtil(arr, out, tmp, i + 1, end, index + 1, r);
		}
	}


	template <class T>
	void genKCombination(const std::vector<T> &arr, int r, std::vector<std::vector<T>> &out) {
		T tmp[arr.size()];

		genKCombinationUtil(arr, out, tmp, 0, arr.size() - 1, 0, r);
	}

	template <class T>
	void genCombinationSumUtil(int l, T sum, T K, std::vector<T> &local, std::vector<T> &A, std::vector<std::vector<T>> &out) {
		if (sum == K) {
			out.push_back(local);

			return;
		}

		for (int i = l; i < A.size(); i++) {
			if (sum + A[i] > K)
				continue;

			local.push_back(A[i]);

			genCombinationSumUtil(i + 1, sum + A[i], K, local, A, out);

			local.pop_back();
		}
	}

	template <class T>
	void genCombinationSum(std::vector<T> A, T sum, std::vector<std::vector<T>> &out) {
		sort(A.begin(), A.end());

		std::vector<T> local;

		genCombinationSumUtil(0, 0, sum, local, A, out);
	}

	uint64_t llrand(uint64_t max);

	std::string toHex(const void *buffer, size_t bufferSize);

	std::string md5(const void *buffer, size_t bufferSize, bool toHexString);
	std::string md5(const std::string &str, bool toHexString);

	// Chinese remainder theorem
	// std::pair<num, rem>
	int64_t getMinXCRD(const std::vector<std::pair<int64_t, int64_t>> &data);
}

#endif /* UTILS_UTILS_H_ */
