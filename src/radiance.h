#pragma once
#include "scene.h"
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "random.h"
#include "bvh.h"
#include "triangle.h"
#include "intersect.h"


// ray��������̕��ˋP�x�����߂�
Color radiance(Scene &scene, Ray &ray, Random &rng, const int maxDepth);

// ������̓_���T���v�����O���Ē��ڌ����v�Z����B//DIFFUSE�ʂŗp����
Color direct_radiance_sample(Scene &scene, const Vec &v0,
							 const Vec &normal, const Triangle& objtri, Random &rng);

Color direct_radiance(const Vec &v0, const Vec &normal, const std::shared_ptr<Triangle> ltri,
	const Vec &lpos, const Intersection &lintersect);