#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <cstdarg>

#include <openssl/md5.h>

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


bool utils::isHex(const std::string &s) {
	std::string::const_iterator it = s.begin();

	while (it != s.end() && std::isxdigit(*it)) ++it;

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


std::string utils::string(const char *format, ...) {
	va_list        ap;
	const uint32_t bufferSize = 1024;
	char           buffer[bufferSize];

	va_start(ap, format);
	vsnprintf(buffer, bufferSize, format, ap);
	va_end(ap);

	return std::string(buffer);
}


uint64_t utils::llrand(uint64_t max) {
	uint64_t num = 0;

	int fd = open("/dev/urandom", O_RDONLY);
	if (fd >= 0) {
		read(fd, &num, sizeof(num));

		close(fd);
	};

	return num % max;
}


static char _hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };


std::string utils::toHex(const void *buffer, size_t bufferSize) {
	std::string ret;

	if (bufferSize > 0) {
		const uint8_t *ptr = reinterpret_cast<const uint8_t *>(buffer);

		ret.reserve(bufferSize * 2);

		for (size_t i = 0; i < bufferSize; i++) {
			ret.push_back(_hex[ptr[i] >> 4]);
			ret.push_back(_hex[ptr[i] & 0x0f]);
		}
	}

	return ret;
}


std::string utils::md5(const void *buffer, size_t bufferSize, bool toHexString) {
	std::string ret;

	ret.reserve(MD5_DIGEST_LENGTH);

	{
		unsigned char mdBuffer[MD5_DIGEST_LENGTH];

		MD5((const unsigned char *) buffer, bufferSize, mdBuffer);

		ret.assign((char *) mdBuffer, MD5_DIGEST_LENGTH);

		if (toHexString) {
			ret = toHex(ret.data(), ret.length());
		}
	}

	return ret;
}


std::string utils::md5(const std::string &str, bool toHexString) {
	return md5(str.data(), str.length(), toHexString);
}


void utils::genCombinationsUtil(std::vector<std::vector<int> > &combinations, std::vector<int> &tmp, int n, int left, int k) {
	if (k == 0) {
		combinations.push_back(tmp);

		return;
	}

	for (int i = left; i <= n; ++i) {
		tmp.push_back(i);
		genCombinationsUtil(combinations, tmp, n, i + 1, k - 1);
		tmp.pop_back();
	}
}


void utils::genCombinations(std::vector<std::vector<int>> &out, int n, int k) {
	std::vector<int> tmp;

	out.clear();

	genCombinationsUtil(out, tmp, n, 1, k);
}


int64_t utils::getMinXCRD(const std::vector<std::pair<int64_t, int64_t>> &data) {
	int x = 1;

	// As per the Chinese remainder theorem,
	// this loop will always break.
	while (true) {
		int j;

		for (j = 0; j < data.size(); j++) {
			if (x % data[j].first != data[j].second) {
				break;
			}
		}

		if (j == data.size()) {
			return x;
		}

		x++;
	}

	return x;
}
