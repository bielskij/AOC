#include <functional>
#include <memory>

#include "common/types.h"

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct Coprocessor {
	class Reg {
		public:
			Reg() {
				this->reset();
			}

			void reset() {
				this->_val = 0;
			}

			int64_t val() const {
				return this->_val;
			}

			void val(int64_t v) {
				this->_val = v;
			}

		private:
			int64_t _val;
	};

	class Regs {
		public:
			Regs() {
				for (int i = 0; i < 8; i++) {
					this->_regs.push_back(Reg());
				}
			}

			Reg &fromChar(char c) {
				if (c < 'a' || c >= 'h') {
					abort();
				}
				return _regs[c - 'a'];
			}

			void reset() {
				for (auto &r : this->_regs) {
					r.reset();
				}
			}

		private:
			std::vector<Reg> _regs;
	};

	class Operand {
		public:
			virtual ~Operand() {}

		public:
			virtual bool isReg() const = 0;
			virtual int64_t val() const  = 0;
			virtual void val(int64_t v) = 0;
	};

	class RegOperand : public Operand {
		public:
			RegOperand(Reg &r) : r(r) {
			}

			virtual bool isReg() const override {
				return true;
			}

			virtual int64_t val() const override {
				return this->r.val();
			}

			virtual void val(int64_t v) override {
				this->r.val(v);
			}

		private:
			Reg &r;
	};

	class ImmOperand : public Operand {
		public:
			ImmOperand(int64_t val) {
				this->_v = val;
			}

			virtual bool isReg() const override {
				return false;
			}

			virtual int64_t val() const override {
				return this->_v;
			}

			virtual void val(int64_t v) override {
				abort();
			}

		private:
			int64_t _v;
	};

	Regs regs;
	int pc;
	std::vector<std::string> code;
	std::map<std::string, std::function<int(Operand &, const Operand &)>> opcodes;

	Coprocessor() {
		opcodes["set"] = [this](Operand &x, const Operand &y) {
			return this->set(x, y);
		};

		opcodes["sub"] = [this](Operand &x, const Operand &y) {
			return this->sub(x, y);
		};

		opcodes["mul"] = [this](Operand &x, const Operand &y) {
			return this->mul(x, y);
		};

		opcodes["jnz"] = [this](Operand &x, const Operand &y) {
			return this->jnz(x, y);
		};

		this->reset();
	}

	virtual ~Coprocessor() {}

	void load(const std::vector<std::string> &code) {
		this->code = code;
	}

	void reset() {
		this->regs.reset();

		this->pc = 0;
	}

	void run() {
		while (this->pc < this->code.size()) {
			std::unique_ptr<Operand> x;
			std::unique_ptr<Operand> y;

			std::string opName;

			{
				const std::string &inst = this->code[this->pc];

				char op[10];

				char operandRight[10];
				char operandLeft[10];

				if (sscanf(inst.c_str(), "%s %s %s", op, operandLeft, operandRight) != 3) {
					PRINTF(("Unable to parse instruction! '%s'", inst.c_str()));
					abort();
				}

				DBG(("[%d] opcode: '%s', left: '%c', right: '%s'", this->pc, op, operandLeft, operandRight));

				if (::isalpha(operandLeft[0])) {
					x = std::make_unique<RegOperand>(this->regs.fromChar(operandLeft[0]));

				} else {
					x = std::make_unique<ImmOperand>(std::stol(operandLeft));
				}

				if (::isalpha(operandRight[0])) {
					y = std::make_unique<RegOperand>(this->regs.fromChar(operandRight[0]));

				} else {
					y = std::make_unique<ImmOperand>(std::stol(operandRight));
				}

				opName = op;
			}

			auto it = this->opcodes.find(opName);
			if (it == this->opcodes.end()) {
				abort();
			}

			this->pc += it->second(*x, *y);
		}
	}

	virtual int set(Operand &x, const Operand &y) {
		x.val(y.val());

		return 1;
	}

	virtual int sub(Operand &x, const Operand &y) {
		x.val(x.val() - y.val());

		return 1;
	}

	virtual int mul(Operand &x, const Operand &y) {
		x.val(x.val() * y.val());

		return 1;
	}

	virtual int jnz(Operand &x, const Operand &y) {
		if (x.val() != 0) {
			return y.val();
		}

		return 1;
	}
};


class CoprocessorWithStatistics : public Coprocessor {
	public:
		CoprocessorWithStatistics() {
			this->mulCalled = 0;
		}

		int getMulCalled() const {
			return this->mulCalled;
		}

		virtual int mul(Operand &x, const Operand &y) override {
			this->mulCalled++;

			return this->Coprocessor::mul(x, y);
		}

	private:
		int mulCalled;
};


int main(int argc, char *argv[]) {
	auto lines = File::readAllLines(argv[1]);

	{
		CoprocessorWithStatistics c;

		c.load(lines);

		c.run();

		PRINTF(("PART_A: %d", c.getMulCalled()));
	}

	return 0;
}
