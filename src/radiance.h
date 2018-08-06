#pragma once
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "random.h"
#include "bvh.h"
#include "triangle.h"
#include "intersect.h"


// ray•ûŒü‚©‚ç‚Ì•úË‹P“x‚ğ‹‚ß‚é
Color radiance(BVH_node *nodes, Ray &ray, Random &rng, int maxDepth);