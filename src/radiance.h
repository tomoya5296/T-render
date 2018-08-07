#pragma once
#include "scene.h"
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "random.h"
#include "bvh.h"
#include "triangle.h"
#include "intersect.h"


// ray方向からの放射輝度を求める
Color radiance(Scene &scene, Ray &ray, Random &rng, const int maxDepth);

// 光源上の点をサンプリングして直接光を計算する。//DIFFUSE面で用いる
Color direct_radiance_sample(Scene &scene, const Vec &v0,
							 const Vec &normal, const Triangle& objtri, Random &rng);

Color direct_radiance(const Vec &v0, const Vec &normal, const std::shared_ptr<Triangle> ltri,
	const Vec &lpos, const Intersection &lintersect);