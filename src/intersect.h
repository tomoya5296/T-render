#pragma once
#include "ray.h"
#include "triangle.h"
#include "vec.h"
#include "common.h"
#include "bvh.h"

struct Hitpoint {
	double distance;
	Vec normal;
	Vec orienting_normal;
	Vec position;
	Hitpoint() : distance(INF), normal(), orienting_normal(), position() {}
};

struct Intersection {
	Hitpoint hitpoint;
	Intersection(){}
};

bool IntersectAABB(float aabb[2][3], const Ray &ray);

bool intersect(const Ray &ray,
	const std::shared_ptr<Triangle> tri,
	Hitpoint *hitpoint);

std::shared_ptr<Triangle> intersect(BVH_node *nodes,
	int index, const Ray &ray,
	Intersection *intersect);