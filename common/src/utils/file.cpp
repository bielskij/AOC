#include <fstream>

#include "utils/file.h"


std::vector<std::string> File::readAllLines(const std::string &path) {
	std::vector<std::string> ret;

	{
		std::ifstream infile(path);

		std::string line;

		while (std::getline(infile, line)) {
			ret.push_back(line);
		}
	}

	return ret;
}
