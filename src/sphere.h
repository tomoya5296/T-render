#pragma once
#include <cmath>
#include "vec.h"
#include "ray.h"

struct Sphere {
	Sphere(const double radius, const Vec center, Color color) :
		radius(radius), center(center), color(color) {}

	bool intersect(const Ray &ray, double *t) const {
		Vec o2c = center - ray.org;
		const double b = Dot(o2c, ray.dir);
		const double det = b * b - Dot(o2c, o2c) + radius * radius;
		if (det >= 0.0) {
			const double sqdet = std::sqrt(det);
			const double t1 = b - sqdet;
			const double t2 = b + sqdet;
			if (t1 > EPS) {
				*t = t1;
				return true;
			}
			else if (t2 > EPS && t2 < *t) {
				*t = t2;
				return true;
			}
		}
		return false;
	}

	double radius;
	Vec center;
	Color color;
};