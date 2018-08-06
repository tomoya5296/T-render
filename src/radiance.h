#pragma once
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "random.h"
#include "bvh.h"
#include "triangle.h"
#include "intersect.h"


// ray��������̕��ˋP�x�����߂�
Color radiance(BVH_node *nodes, Ray &ray, Random &rng, int maxDepth);