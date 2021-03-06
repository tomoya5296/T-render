#pragma once
#ifndef  _VEC_H_
#define _VEC_H_

#include <iostream>
#include<sstream>
#include<cmath>
#include "common.h"

struct Vec;
using Color = Vec;

struct Vec {
	explicit Vec(const double x_ = 0.0)
		: x(x_), y(x_), z(x_) {
	}

	Vec(const double x_, const double y_, const double z_)
		: x(x_), y(y_), z(z_) {
	}

	Vec(const Vec &b)
		: x(b.x), y(b.y), z(b.z) {
	}

	inline bool operator==(const Vec &b) {
		return (x == b.x) && (y == b.y) && (z == b.z);
	}

	inline bool operator!=(const Vec &b) {
		return (x != b.x) || (y != b.y) || (z != b.z);
	}

	inline Vec& operator=(const Vec &b) {
		this->x = b.x;
		this->y = b.y;
		this->z = b.z;
		return *this;
	}

	inline Vec operator+(const Vec &b) const { return Vec(x + b.x, y + b.y, z + b.z); }
	inline Vec operator-() const { return Vec(-x, -y, -z); }
	inline Vec operator-(const Vec &b) const { return Vec(x - b.x, y - b.y, z - b.z); }
	inline Vec operator*(double b) const { return Vec(x * b, y * b, z * b); }

	inline Vec operator/(double b) const {
		Assertion(b != 0.0, "Zero division!!");
		return Vec(x / b, y / b, z / b);
	}

	inline Vec operator/(const Vec &b) const {
		Assertion(b.x != 0.0, "Zero division!!");
		Assertion(b.y != 0.0, "Zero division!!");
		Assertion(b.z != 0.0, "Zero division!!");
		return Vec(x / b.x, y / b.y, z / b.z);
	}

	inline Vec operator*(const Vec &b) const {
		return Vec(x*b.x, y*b.y, z*b.z);
	}

	inline double LengthSquared() const {
		return x * x + y * y + z * z;
	}

	inline double Length() const {
		return std::sqrt(LengthSquared());
	}

	static Vec exp(const Vec &v) {
		return Vec(std::exp(v.x), std::exp(v.y), std::exp(v.z));
	}

	static Vec sqrt(const Vec &v) {
		return Vec(std::sqrt(v.x), std::sqrt(v.y), std::sqrt(v.z));
	}

	bool isZero() const {
		return x == 0.0 && y == 0.0 && z == 0.0;
	}

	bool isValid() const {
		if (std::isinf(x) || std::isnan(x)) return false;
		if (std::isinf(y) || std::isnan(y)) return false;
		if (std::isinf(z) || std::isnan(z)) return false;
		return true;
	}

	std::string toString() const {
		std::stringstream ss;
		ss << "( " << x << ", " << y << ", " << z << " )";
		return ss.str();
	}

	double x, y, z;
};

inline Vec operator*(double f, const Vec &v) { return v * f; }
inline Vec Normalize(const Vec &v) { return v / (v.Length()); }
inline Vec reflect(const Vec &dir, const Vec &normal) {
	double dot = dir.x * normal.x
		+ dir.y * normal.y
		+ dir.z * normal.z;

	return Vec(dir.x - 2.0 * dot * normal.x,
		dir.y - 2.0 * dot * normal.y,
		dir.z - 2.0 * dot * normal.z);
}
inline std::ostream & operator<<(std::ostream &os, const Vec &v) {
	os << v.toString();
	return os;
}

inline double Dot(const Vec &v1, const Vec &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vec Cross(const Vec &v1, const Vec &v2) {
	return Vec((v1.y * v2.z) - (v1.z * v2.y),
			   (v1.z * v2.x) - (v1.x * v2.z),
			   (v1.x * v2.y) - (v1.y * v2.x));
}

inline double det(const Vec &vecA, const Vec &vecB, const Vec &vecC) {
	return ((vecA.x * vecB.y * vecC.z)
		+ (vecA.y * vecB.z * vecC.x)
		+ (vecA.z * vecB.x * vecC.y)
		- (vecA.x * vecB.z * vecC.y)
		- (vecA.y * vecB.x * vecC.z)
		- (vecA.z * vecB.y * vecC.x));
}

#endif // ! _VEC_H_
