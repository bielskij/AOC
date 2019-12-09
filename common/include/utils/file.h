#ifndef UTILS_FILE_H_
#define UTILS_FILE_H_

#include <string>
#include <vector>

class File {
	public:
		static std::vector<std::string> readAllLines(const std::string &path);
};

#endif /* UTILS_FILE_H_ */
