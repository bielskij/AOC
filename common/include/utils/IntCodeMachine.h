#ifndef UTILS_INTCODEMACHINE_H_
#define UTILS_INTCODEMACHINE_H_

#include <vector>


class IntCodeMachine {
	public:
		IntCodeMachine(const std::vector<int> &program);
		virtual ~IntCodeMachine();

		bool run();

		void reset();
		bool finished() const;

		std::vector<int> &getMemory();

	protected:
		virtual bool onOut(int value);
		virtual bool onIn(int &value);

	private:
		bool handleOpcode(std::vector<int> &mem, int code, int arg1, int arg2, int arg3);

	private:
		bool eop;
		int pc;
		std::vector<int> memory;
		std::vector<int> program;
};


#endif /* UTILS_INTCODEMACHINE_H_ */
