#pragma once
#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "vec.h"
#include "common.h"


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

#endif