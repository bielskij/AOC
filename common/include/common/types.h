#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <cmath>

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

		bool operator!=(const Point &other) {
			return !(*this == other);
		}

	private:
		float x;
		float y;
};

template <class T>
class Point3d {
	public:
		Point3d() {
			this->n[0] = 0;
			this->n[1] = 0;
			this->n[2] = 0;
		}

		Point3d(T x, T y, T z) {
			this->n[0] = x;
			this->n[1] = y;
			this->n[2] = z;
		}

		virtual ~Point3d() {
		}

		T getX() const {
			return this->n[0];
		}

		T getY() const {
			return this->n[1];
		}

		T getZ() const {
			return this->n[2];
		}

		T get(int idx) {
			return this->n[idx];
		}

		void setX(T x) {
			this->n[0] = x;
		}

		void setY(T y) {
			this->n[1] = y;
		}

		void setZ(T z) {
			this->n[2] = z;
		}

		void set(int idx, T v) {
			this->n[idx] = v;
		}

		bool operator==(const Point3d &other) {
			return (this->n[0] == other.n[0]) && (this->n[1] == other.n[1]) && (this->n[2] == other.n[2]);
		}

		bool operator!=(const Point3d &other) {
			return !(*this == other);
		}

	private:
		T n[3];
};


class Line {
	public:
		Line(const Point &begin, const Point &end) {
			this->a = end.getY() - begin.getY();
			this->b = begin.getX() - end.getX();
			this->c = a * begin.getX() + b * begin.getY();
		}

		float value(float x) {
			// ax + by  =c
			return (c - a * x) / b;
		}

		bool crossTrough(const Point &p) {
			return this->a * p.getX() + this->b * p.getY() == this->c;
		}

	private:
		float a;
		float b;
		float c;
};


class Vector2D {
	public:
		Vector2D(float x, float y) {
			this->x = x;
			this->y = y;
		}

		double getX() const {
			return this->x;
		}

		double getY() const {
			return this->y;
		}

		double getSquaredLength() const {
				return (*this) * (*this);
		}

		double getLength() const {
			return sqrt(getSquaredLength());
		}

		Vector2D normalized() const {
			return *this / getLength();
		}

		double getAngle(const Vector2D &other) const {
			double abs = acos(normalized() * other.normalized());

			abs = 180.0f * abs / M_PI;

			return x * other.y - y * other.x >= 0.0f ? abs : -abs;
		}

		double operator*(const Vector2D &other) const {
			return this->x * other.x + this->y * other.y;
		}

		const Vector2D operator-() const {
			return Vector2D(-this->x, -this->y);
		}

		const Vector2D operator/(const double &scalar) const {
			return Vector2D(x / scalar, y / scalar);
		}

	private:
		double x;
		double y;
};


#endif /* COMMON_TYPES_H_ */
