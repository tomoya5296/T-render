#pragma once
#include "vec.h"

class Camera
{
public:
	Camera() {
		pos = 0.1 * Vec(49.0, 60.0, 295.6);
		dir = Normalize(Vec(-0.045, -0.042612, -1.0));
	}

	Vec pos, dir;
};
