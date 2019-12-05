#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

class Point {
	public:
		Point() {
			this->x = 0;
			this->y = 0;
		}

		Point(int x, int y) {
			this->x = x;
			this->y = y;
		}

		virtual ~Point() {
		}

		float getX() const {
			return this->x;
		}

		float getY() const {
			return this->y;
		}

		void setX(float x) {
			this->x = x;
		}

		void setY(float y) {
			this->y = y;
		}

		bool operator==(const Point &other) {
			return this->x == other.x && this->y == other.y;
		}

	private:
		float x;
		float y;
};

#endif /* COMMON_TYPES_H_ */
