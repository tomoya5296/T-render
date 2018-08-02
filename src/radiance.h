#pragma once
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "random.h"
#include "bvh.h"
#include "triangle.h"
#include "intersect.h"


// ray��������̕��ˋP�x�����߂�
//Color radiance(BVH_node *nodes, Ray &ray, Random &rng, int maxDepth) {
//
//	int depth;
//	Color L = Color(0.0), beta = Color(1.0);
//	float pdf = 1.0;
//
//	for (depth = 0;; ++depth) {
//		std::shared_ptr<Triangle> HitTri = nullptr;
//		Intersection intersection;
//		HitTri = intersect(nodes, 0, ray, &intersection);
//		/* �ő��bounce����]��*/
//		if (depth >= maxDepth) {
//			L = Color(0.0);
//			break;
//		}
//
//		if (HitTri == nullptr) {
//			L = Color(0.0);
//			break;
//		}
//		const double t = intersection.hitpoint.distance;
//		const Triangle &obj = *HitTri;
//		const Vec normal = obj.normal;
//		const Vec orienting_normal = Dot(normal, ray.dir) < 0.0 ? normal : (-1.0 * normal); // �����ʒu�̖@���i���̂���̃��C�̓��o���l���j
//		const Vec hitpoint = ray.org + (t - EPS) * ray.dir;
//
//		// �Œ�o�E���X��]�����ȏヌ�C��ǐՂ����烍�V�A�����[���b�g�����s���ǐՂ�ł��؂邩�ǂ����𔻒f����
//		double russian_roulette_probability = std::max(obj.mat.ref.x, std::max(obj.mat.ref.y, obj.mat.ref.z));
//		russian_roulette_probability = std::max(0.05, russian_roulette_probability);
//		if (depth > 5) {
//			if (rng.next01() > russian_roulette_probability) {
//				/*	Vec direct_light;
//				if (obj.obj_id != LightID) {
//				const int shadow_ray = 1;
//				for (int i = 0; i < shadow_ray; i++) {
//				direct_light = direct_light + direct_radiance_sample(hitpoint, orienting_normal, &obj, rng.next01(), rng.next01(), rng.next01()) / shadow_ray;
//				}
//				return direct_light/ (1.0 - russian_roulette_probability);
//				}*/
//				//else {
//				/*if (Dot(-ray.dir, normal) > 0.0) {
//				return obj.mat.Le / (1.0 - russian_roulette_probability);
//				}
//				else {*/
//				L = Color(0.0);
//				break;
//				//}
//				//	}
//			}
//		}
//		else {
//			russian_roulette_probability = 1.0;
//		}
//
//		// ���C�ƕ��̂̌����_����̕��ˋP�x�`�B���v�Z
//		switch (obj.mat.type) {
//		case DIFFUSE: {
//			// ���ڌ��̃T���v�����O���s��
//			if (!obj.mat.isLight()) {
////				const int shadow_ray = 1;
////				Vec direct_light;
////				for (int i = 0; i < shadow_ray; i++) {
////					direct_light = direct_light + direct_radiance_sample(hitpoint, orienting_normal, &obj, rng.next01(), rng.next01(), rng.next01()) / shadow_ray;
////				}
//
//				// orienting_normal�̕�������Ƃ������K�������(w, u, v)�����B���̊��ɑ΂��锼�����Ŏ��̃��C���΂��B
//				Vec w, u, v;
//				w = orienting_normal;
//				if (std::abs(w.x) > 0.1)
//					u = Normalize(Cross(Vec(0.0, 1.0, 0.0), w));
//				else
//					u = Normalize(Cross(Vec(1.0, 0.0, 0.0), w));
//				v = Cross(w, u);
//
//				// �R�T�C�������g�����d�_�I�T���v�����O
//				const double r1 = 2.0 * PI * rng.next01();
//				const double r2 = rng.next01();
//				const double r2s = sqrt(r2);
//				Vec dir = Normalize((u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1.0 - r2)));
//				ray = Ray(hitpoint, dir);
//				beta *= (obj.mat.ref / russian_roulette_probability);
//				//L += direct_light + (obj.mat.ref * radiance(nodes, Ray(hitpoint, dir), rng, depth + 1, maxDepth)) / russian_roulette_probability;
//				//break;
//			}
//			else if (depth == 0) {
//				if (Dot(-ray.dir, obj.normal) > 0.0)
//					L = obj.mat.Le;
//				else
//					L = Color(0.0);
//				break;
//			}
//			else {
//				if (Dot(-ray.dir, obj.normal) > 0.0) 
//					L = obj.mat.Le * beta;
//				else 
//					L = Color(0.0);
//				break;
//			}
//		} 
//			//case SPECULAR: {
//			//	// ���S���ʂȂ̂Ń��C�̔��˕����͌���I�B
//			//	// ���V�A�����[���b�g�̊m���ŏ��Z����̂͏�Ɠ����B
//			//	//Intersection lintersect;//���ˌ��̏��
//			//	Ray reflection_ray = Ray(hitpoint, Normalize(ray.dir + normal * 2.0 * Dot(-normal, ray.dir)));
//			//	TRIANGLE* lHitTri = nullptr;
//			//	Intersection lintersection;
//			//	lHitTri = Intersect(nodes, 0, reflection_ray, &lintersection);
//			//	Vec direct_light = Color();
//			//	if (lintersection.obj_id == LightID) {
//			//		Vec Light_pos = hitpoint + lintersection.hitpoint.distance*reflection_ray.dir;
//			//		direct_light = Multiply(HitTri->mat.ref, triangles[LightID][0].mat.Le);
//			//	}
//			//	return direct_light
//			//		+ Multiply(transmittance_ratio, radiance(reflection_ray, medium, rng, depth + 1, maxDepth)) / (1.0 - scattering_probability) / russian_roulette_probability;
//			//} break;
//			//case REFRACTION: {
//			//	Ray reflection_ray = Ray(hitpoint, ray.dir - normal * 2.0 * Dot(normal, ray.dir));
//			//	// ���˕�������̒��ڌ��T���v�����O����
//			//	Intersection llintersect;
//			//	TRIANGLE* lHitTri = nullptr;
//			//	lHitTri = Intersect(nodes, 0, reflection_ray, &llintersect);
//			//	//intersect_scene_triangle(reflection_ray, &lintersect);
//			//	Vec direct_light;
//			//	if (llintersect.obj_id == LightID) {
//			//		direct_light = Multiply(HitTri->mat.ref, triangles[LightID][0].mat.Le);
//			//	}
//			//	bool into = Dot(normal, orienting_normal) > 0.0; // ���C���I�u�W�F�N�g����o��̂��A����̂��A�o��Ȃ��false
//			//													 // Snell�̖@��
//			//	const double nc = 1.0; // �^��̋��ܗ�
//			//	const double nt = 1.5; // �I�u�W�F�N�g�̋��ܗ�
//			//	const double nnt = into ? nc / nt : nt / nc;
//			//	const double ddn = Dot(ray.dir, orienting_normal);
//			//	const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
//			//	if (cos2t < 0.0) { // �S���˂���
//			//		return direct_light + Multiply(transmittance_ratio, Multiply(obj.mat.ref, (radiance(reflection_ray, medium, rng, depth + 1, maxDepth)))) / (1.0 - scattering_probability) / russian_roulette_probability;
//			//	}
//			//	// ���܂��Ă�������
//			//	Vec tdir = Normalize(ray.dir * nnt - normal * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t)));
//			//	// Schlick�ɂ��Fresnel�̔��ˌW���̋ߎ�
//			//	const double a = nt - nc, b = nt + nc;
//			//	const double R0 = (a * a) / (b * b);
//			//	const double c = 1.0 - (into ? -ddn : Dot(tdir, normal));
//			//	const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
//			//	const double Tr = 1.0 - Re; // ���܌��̉^�Ԍ��̗�
//			//	const double probability = 0.25 + 0.5 * Re;
//			//	// ���ܕ�������̒��ڌ��T���v�����O����
//			//	Ray refraction_ray = Ray(hitpoint + 2 * EPS*ray.dir, tdir);
//			//	Intersection lintersect;
//			//	lHitTri = nullptr;
//			//	lHitTri = Intersect(nodes, 0, refraction_ray, &lintersect);
//			//	Vec direct_light_refraction;
//			//	if (lintersect.obj_id == LightID) {
//			//		direct_light_refraction = Multiply(HitTri->mat.ref, triangles[LightID][0].mat.Le);
//			//	}
//			//	// ���ȏヌ�C��ǐՂ�������܂Ɣ��˂̂ǂ��炩�����ǐՂ���B�i�����Ȃ��Ǝw���I�Ƀ��C��������j
//			//	// ���V�A�����[���b�g�Ō��肷��B
//			//	if (depth > 4) {
//			//		if (rng.next01() < probability) { // ����
//			//			return direct_light +
//			//				Multiply(transmittance_ratio, Multiply(obj.mat.ref, radiance(reflection_ray, medium, rng, depth + 1, maxDepth) * Re))
//			//				/ probability
//			//				/ (1.0 - scattering_probability)
//			//				/ russian_roulette_probability;
//			//		}
//			//		else { // ����
//			//			return direct_light_refraction +
//			//				Multiply(transmittance_ratio, Multiply(obj.mat.ref, radiance(refraction_ray, medium, rng, depth + 1, maxDepth) * Tr))
//			//				/ (1.0 - probability)
//			//				/ (1.0 - scattering_probability)
//			//				/ russian_roulette_probability;
//			//		}
//			//	}
//			//	else {
//			//		return direct_light + direct_light_refraction +
//			//			Multiply(transmittance_ratio, Multiply(obj.mat.ref, radiance(reflection_ray, medium, rng, depth + 1, maxDepth) * Re
//			//				+ radiance(refraction_ray, medium, rng, depth + 1, maxDepth) * Tr)) / (1.0 - scattering_probability) / russian_roulette_probability;
//			//	}
//			//}break;
//		}
//	}
//
//	return L;
//}