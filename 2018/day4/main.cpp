#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <cstring>
#include <vector>

#include "utils/file.h"
#include "utils/utils.h"

#define DEBUG_LEVEL 5
#include "common/debug.h"


struct DateTime {
	int year;
	int month;
	int day;
	int hour;
	int minute;

	void reset() {
		this->year   = 0;
		this->month  = 0;
		this->day    = 0;
		this->hour   = 0;
		this->minute = 0;
	}

	int minuteDiff(const DateTime &other) {
		long int lts = this->month * 31 * 24 * 60 + this->day * 24 * 60 + this->hour * 60 + this->minute;
		long int rts = other.month * 31 * 24 * 60 + other.day * 24 * 60 + other.hour * 60 + other.minute;

		if (rts > lts) {
			return rts - lts;
		} else {
			return lts - rts;
		}
	}
};

struct DateTimeRange {
	DateTime start;
	DateTime end;

	DateTimeRange(const DateTime &start, const DateTime &end) : start(start), end(end) {
	}
};

class AsleepReport {
	private:
		std::vector<DateTimeRange> asleepIntervals;

	public:
		virtual ~AsleepReport() {

		}

		void addAsleepRange(const DateTime &start, const DateTime &end) {
			this->asleepIntervals.push_back(DateTimeRange(start, end));
		}

		int getTotalAsleepMinutes() {
			int ret = 0;

			for (auto &i : asleepIntervals) {
				ret += i.end.minuteDiff(i.start);
			}

			return ret;
		}

		const std::vector<DateTimeRange> &getAsleepIntervals() const {
			return asleepIntervals;
		}
};

class Operation {
	DateTime datetime;

	public:
		Operation(const DateTime &dateTime) : datetime(dateTime) {
		}

		virtual ~Operation() {
		}

		const DateTime &getDateTime() const {
			return this->datetime;
		}
};


class OperationBeginShift : public Operation {
	int guardId;

	public:
		OperationBeginShift(const DateTime &dateTime, int guardId) : Operation (dateTime) {
			this->guardId = guardId;
		}

		int getId() const {
			return this->guardId;
		}
};

class OperationWakeUp : public Operation {
	public:
		OperationWakeUp(const DateTime &dateTime) : Operation(dateTime) {

		}
};

class OperationFallAsleep : public Operation {
	public:
		OperationFallAsleep(const DateTime &dateTime) : Operation(dateTime) {

		}
};

struct OperationComparator {
	bool operator() (const Operation *lhs, const Operation *rhs) const {
		const DateTime &ldt = lhs->getDateTime();
		const DateTime &rdt = rhs->getDateTime();

		// Year does not matter
		long int lts = ldt.month * 31 * 24 * 60 + ldt.day * 24 * 60 + ldt.hour * 60 + ldt.minute;
		long int rts = rdt.month * 31 * 24 * 60 + rdt.day * 24 * 60 + rdt.hour * 60 + rdt.minute;

		if (lts < rts) {
			return true;
		}

		return false;
	}
};


int main(int argc, char *argv[]) {
	auto data = File::readAllLines(argv[1]);

	{
		std::set<Operation *, OperationComparator> operations;

		for (auto &line : data) {
			DateTime dt;
			int guardId;
			char word[32];

			if (sscanf(line.c_str(), "[%d-%d-%d %d:%d] Guard #%d begins shift", &dt.year, &dt.month, &dt.day, &dt.hour, &dt.minute, &guardId) == 6) {
				operations.insert(new OperationBeginShift(dt, guardId));

			} else if (sscanf(line.c_str(), "[%d-%d-%d %d:%d] %s %*s", &dt.year, &dt.month, &dt.day, &dt.hour, &dt.minute, word) == 6) {
				if (strcmp(word, "falls") == 0) {
					operations.insert(new OperationFallAsleep(dt));

				} else if (strcmp(word, "wakes") == 0) {
					operations.insert(new OperationWakeUp(dt));

				} else {
					ERR(("Not supported word: %s", word));
					abort();
				}

			} else {
				ERR(("Not supported line: '%s'\n", line.c_str()));
				abort();
			}
		}

		std::map<int, AsleepReport> guards;

		{
			AsleepReport *currentReport = nullptr;
			DateTime      currentSleepStart;

			for (auto operation : operations) {
				if (dynamic_cast<OperationBeginShift *>(operation)) {
					OperationBeginShift *op = dynamic_cast<OperationBeginShift *>(operation);

					currentReport = &guards[op->getId()];

				} else if (dynamic_cast<OperationFallAsleep *>(operation)) {
					currentSleepStart = operation->getDateTime();

				} else if (dynamic_cast<OperationWakeUp *>(operation)) {
					currentReport->addAsleepRange(currentSleepStart, operation->getDateTime());
				}
			}
		}

		{
			int guardId = -1;
			int asleepTotal = 0;

			for (auto i : guards) {
				if (i.second.getTotalAsleepMinutes() > asleepTotal) {
					asleepTotal = i.second.getTotalAsleepMinutes();
					guardId     = i.first;
				}
			}

			int minuteMap[60];

			memset(minuteMap, 0, sizeof(minuteMap));

			for (const auto i : guards.at(guardId).getAsleepIntervals()) {
				int startMinute = i.start.minute;
				int endMinute   = i.end.minute;

				if (endMinute > startMinute) {
					for (int j = startMinute; j < endMinute; j++) {
						minuteMap[j]++;
					}

				} else {
					for (int j = endMinute; j < 60; j++) {
						minuteMap[j]++;
					}

					for (int j = 0; j < startMinute; j++) {
						minuteMap[j]++;
					}
				}

			}

			{
				int maxMinute    = 0;
				int maxMinuteVal = 0;

				for (int i = 0; i < 60; i++) {
					if (minuteMap[i] > maxMinuteVal) {
						maxMinuteVal = minuteMap[i];
						maxMinute    = i;
					}
				}

				PRINTF(("PART_A: minute: %d, id: %d, mul: %d", maxMinute, guardId, maxMinute * guardId));
			}
		}

		{
			int guardId     = -1;
			int guardMinute = 0;
			int guardCount  = 0;

			for (const auto guard : guards) {
				int minuteMap[60];

				memset(minuteMap, 0, sizeof(minuteMap));

				for (const auto interval : guard.second.getAsleepIntervals()) {
					int startMinute = interval.start.minute;
					int endMinute   = interval.end.minute;

					if (endMinute > startMinute) {
						for (int j = startMinute; j < endMinute; j++) {
							minuteMap[j]++;
						}

					} else {
						for (int j = endMinute; j < 60; j++) {
							minuteMap[j]++;
						}

						for (int j = 0; j < startMinute; j++) {
							minuteMap[j]++;
						}
					}
				}

				{
					int maxMinute    = 0;
					int maxMinuteVal = 0;

					for (int i = 0; i < 60; i++) {
						if (minuteMap[i] > maxMinuteVal) {
							maxMinuteVal = minuteMap[i];
							maxMinute    = i;
						}
					}

					if (maxMinuteVal > guardCount) {
						guardCount  = maxMinuteVal;
						guardMinute = maxMinute;
						guardId     = guard.first;
					}
				}
			}

			PRINTF(("PART_B: Guard id: %d, mul: %d", guardId, guardId * guardMinute));
		}
	}

	return 0;
}
