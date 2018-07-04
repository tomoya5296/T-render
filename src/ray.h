#pragma once
#include "vec.h"

struct Ray
{
	Ray(const Vec &org_, const Vec &dir_) :
		org(org_), dir(dir_) {}

	Vec org, dir;
};
