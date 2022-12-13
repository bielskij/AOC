#include <stack>
#include <cassert>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"

struct List;


struct Node {
	virtual ~Node() {}

	virtual List *toList() = 0;

	virtual bool isList() = 0;
};


struct List {
	virtual ~List() {}

	virtual Node *at(int idx) = 0;
	virtual int size() = 0;
	virtual void push(Node *n) = 0;
};


struct ValueNode : public Node, public List {
	int value;

	ValueNode(int v) {
		this->value = v;
	}

	virtual List *toList() override {
		return this;
	}

	virtual bool isList() override {
		return false;
	}

	virtual Node *at(int ifx) override {
		return this;
	}

	virtual int size() override {
		return 1;
	}

	virtual void push(Node *n) override {
		assert(false);
	};
};



struct ListNode : public Node, public List {
	std::vector<Node *> nodes;

	virtual List *toList() override {
		return this;
	}

	virtual bool isList() override {
		return true;
	}

	virtual Node *at(int ifx) override {
		return nodes[ifx];
	}

	virtual int size() override {
		return nodes.size();
	}

	virtual void push(Node *n) override {
		this->nodes.push_back(n);
	}
};


static Node *parse(const std::string &str) {
	Node *ret = nullptr;

	{
		std::stack<Node *> nodes;

		int value = -1;

		for (char c : str) {
			switch (c) {
				case '[':
					{
						Node *node = new ListNode();

						if (! nodes.empty()) {
							nodes.top()->toList()->push(node);
						}

						nodes.push(node);

						value = -1;
					}
					break;

				case ',':
					{
						if (value != -1) {
							nodes.top()->toList()->push(new ValueNode(value));

							value = -1;
						}
					}
					break;

				case ']':
					if (value != -1) {
						nodes.top()->toList()->push(new ValueNode(value));

						value = -1;
					}

					if (nodes.size() == 1) {
						ret = nodes.top();
					}

					nodes.pop();
					break;

				default:
					{
						if (value == -1) {
							value = c - '0';

						} else {
							value *= 10;
							value += c - '0';
						}
					}
					break;
			}
		}
	}

	return ret;
}


int compare(Node *l, Node *r) {
	int ret = 2;

	// value
	if (! l->isList() && ! r->isList()) {
		auto *lv = reinterpret_cast<ValueNode *>(l);
		auto *rv = reinterpret_cast<ValueNode *>(r);

		if (lv->value == rv->value) {
			ret = 2;

		} else if (lv->value < rv->value) {
			ret = 1;

		} else {
			ret = 0;
		}

	// lists or mixed
	} else if (l->isList() || r->isList()) {
		List *ll = l->toList();
		List *rl = r->toList();

		for (int i = 0; i < std::min(ll->size(), rl->size()); i++) {
			ret = compare(ll->at(i), rl->at(i));
			if (ret != 2) {
				break;
			}
		}

		if (ret == 2) {
			if (ll->size() < rl->size()) {
				ret = 1;

			} else if (ll->size() > rl->size()) {
				ret = 0;
			}
		}
	}

	return ret;
}


bool solve(std::vector<std::string> &pair) {
	Node *left  = parse(pair[0]);
	Node *right = parse(pair[1]);

	return compare(left, right) == 1;
}


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		int partA = 0;
		int pairNo = 1;

		std::vector<std::string> pair;
		for (auto &line : lines){
			if (line.empty()) {
				if (! pair.empty()) {
					if (solve(pair)) {
						partA += pairNo;
					}

					pairNo++;
					pair.clear();
				}

			} else {
				pair.push_back(line);
			}
		}

		if (! pair.empty()) {
			if (solve(pair)) {
				partA += pairNo;
			}
		}

		PRINTF(("PART_A: %d", partA));
	}

	{
		std::vector<std::string> packets;

		for (auto &l : lines) {
			if (! l.empty()) {
				packets.push_back(l);
			}
		}

		packets.push_back("[[2]]");
		packets.push_back("[[6]]");

		std::sort(packets.begin(), packets.end(), [](const std::string &l, const std::string &r) {
			int ret = compare(parse(l), parse(r));

			return ret == 1;
		});

		int partB = 1;

		for (size_t i = 0; i < packets.size(); i++) {
			if (packets[i] == "[[2]]" || packets[i] == "[[6]]") {
				partB *= (i + 1);
			}
		}

		PRINTF(("PART_B: %d", partB));
	}

	return 0;
}
