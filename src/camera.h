#pragma once
#include "vec.h"

class Camera
{
public:
	Camera() {
		pos = Vec(4.0, 6.0, 30.0);
		dir = Normalize(Vec(0.0, 0.0, -1.0));
	}
	Vec pos, dir;
};
