#include <filesystem>
#include <cstring>
#include <limits>
#include <assert.h>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

struct Directory;

struct Node {
	std::string name;
	Node *parent;

	Node(const std::string &name, Node *parent) : name(name) {
		this->parent = parent;
	}

	virtual ~Node() {}

	virtual bool isDir() const = 0;
	virtual bool isFile() const = 0;
};

struct File_ : public Node {
	size_t size;

	File_(const std::string &name, Node *parent, size_t fs) : Node(name, parent) {
		this->size = fs;
	}

	virtual bool isDir() const {
		return false;
	}
	virtual bool isFile() const {
		return true;
	}

	static File_ *from(Node *from) {
		return reinterpret_cast<File_ *>(from);
	}
};


struct Directory : public Node {
	std::vector<Node *> files;

	Directory(const std::string &name, Directory *parent) : Node(name, parent) {
	}

	virtual bool isDir() const override{
		return true;
	}
	virtual bool isFile() const override {
		return false;
	}

	static Directory *from(Node *from) {
		return reinterpret_cast<Directory *>(from);
	}
};


static size_t du(Directory *d) {
	size_t ret = 0;

	for (auto &f : d->files) {
		if (f->isDir()) {
			ret += du(Directory::from(f));

		} else {
			ret += File_::from(f)->size;
		}
	}

	return ret;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	Directory *root = new Directory("/", nullptr);

	std::vector<Directory *> dirs = { root };

	Directory *currentDir = root;

	for (auto &l : lines) {
		auto toks = utils::strTok(l, ' ');

		if (toks.size() == 3) {
			assert(toks[1] == "cd");

			if (toks[2] == "..") {
				currentDir = Directory::from(currentDir->parent);

			} else {
				if (toks[2] == "/") {
					currentDir = root;
				} else {
					for (auto *n : currentDir->files) {
						if (n->name == toks[2]) {
							assert(n->isDir());

							currentDir = Directory::from(n);
							break;
						}
					}
				}
			}

		} else if (toks[1] != "ls") {
			if (toks[0] == "dir") {
				auto newDir = new Directory(toks[1], currentDir);

				currentDir->files.push_back(newDir);

				dirs.push_back(newDir);

			} else {
				currentDir->files.push_back(new File_(toks[1], currentDir, utils::toInt(toks[0])));
			}
		}
	}

	{
		size_t partA = 0;

		for (auto *n : dirs) {
			auto totalSize = du(reinterpret_cast<Directory *>(n));

			if (totalSize <= 100000) {
				partA += totalSize;
			}
		}

		PRINTF(("PART_A: %lld", (long long int) partA));
	}

	{
		size_t toFree = 30000000 - (70000000 - du(root));

		size_t partB = std::numeric_limits<size_t>::max();

		for (auto *n : dirs) {
			auto totalSize = du(reinterpret_cast<Directory *>(n));

			if (totalSize >= toFree) {
				if (totalSize < partB) {
					partB = totalSize;
				}
			}
		}

		PRINTF(("PART_B: %zd", partB));
	}

	return 0;
}
