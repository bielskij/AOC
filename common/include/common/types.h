#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

#include <cmath>

template <class T>
class Point {
	public:
		Point() {
			this->_x = 0;
			this->_y = 0;
		}

		Point(T x, T y) {
			this->_x = x;
			this->_y = y;
		}

		virtual ~Point() {
		}

		T x() const {
			return this->_x;
		}

		T y() const {
			return this->_y;
		}

		void x(T x) {
			this->_x = x;
		}

		void y(T y) {
			this->_y = y;
		}

		void set(T x, T y) {
			this->x(x);
			this->y(y);
		}

		void incX() {
			this->_x++;
		}

		void decX() {
			this->_x--;
		}

		void incY() {
			this->_y++;
		}

		void decY() {
			this->_y--;
		}

		bool operator==(const Point &other) const {
			return this->_x == other._x && this->_y == other._y;
		}

		bool operator!=(const Point &other) const {
			return !(*this == other);
		}

		Point operator+(const Point &other) {
			return Point(this->x() + other.x(), this->y() + other.y());
		}

		Point &operator=(const Point &other) {
			this->_x = other._x;
			this->_y = other._y;

			return *this;
		}

	private:
		T _x;
		T _y;
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


template <class T>
class Line {
	public:
		Line(const Point<T> &begin, const Point<T> &end) : start(begin), end(end) {
			this->a = end.y() - begin.y();
			this->b = begin.x() - end.x();
			this->c = a * begin.x() + b * begin.y();
		}

		float value(float x) {
			// ax + by  =c
			return (c - a * x) / b;
		}

		bool crossTrough(const Point<T> &p) {
			return this->a * p.x() + this->b * p.y() == this->c;
		}

		bool crossTrough(const Line<T> &line, Point<T> &point) {
			float uA = ((line.end.x() - line.start.x()) * (this->start.y() - line.start.y()) - (line.end.y() - line.start.y()) * (this->start.x() - line.start.x())) / ((line.end.y() - line.start.y()) * (this->end.x() - this->start.x()) - (line.end.x() - line.start.x()) * (this->end.y() - this->start.y()));
			float uB = ((this->end.x() - this->start.x()) * (this->start.y() - line.start.y()) - (this->end.y() - this->start.y()) * (this->start.x() - line.start.x())) / ((line.end.y() - line.start.y()) * (this->end.x() - this->start.x()) - (line.end.x() - line.start.x()) * (this->end.y() - this->start.y()));

			if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
				point.x(this->start.x() + (uA * (this->end.x() - this->start.x())));
				point.y(this->start.y() + (uA * (this->end.y() - this->start.y())));

				return true;
			}

			return false;
		}

		Point<T> getStart() const {
			return this->start;
		}

		Point<T> getEnd() const {
			return this->end;
		}

	private:
		float a;
		float b;
		float c;

		Point<T> start;
		Point<T> end;
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
