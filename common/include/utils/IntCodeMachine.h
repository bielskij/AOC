#ifndef UTILS_INTCODEMACHINE_H_
#define UTILS_INTCODEMACHINE_H_

#include <stdint.h>
#include <vector>


class IntCodeMachine {
	public:
		IntCodeMachine(const std::vector<int64_t> &program);
		virtual ~IntCodeMachine();

		bool run();

		void reset();
		bool finished() const;

		std::vector<int64_t> &getMemory();

	protected:
		virtual bool onOut(int64_t value);
		virtual bool onIn(int64_t &value);

	private:
		bool handleOpcode(std::vector<int64_t> &mem, int code, int64_t arg1, int64_t arg2, int64_t arg3);

	private:
		bool eop;
		int64_t pc;
		int64_t relativeBase;
		std::vector<int64_t> memory;
		std::vector<int64_t> program;
};


#endif /* UTILS_INTCODEMACHINE_H_ */
