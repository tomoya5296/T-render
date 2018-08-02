#pragma once
#include "vec.h"
#include "ray.h"
#include "material.h"
#include "random.h"
#include "bvh.h"
#include "triangle.h"
#include "intersect.h"


// ray方向からの放射輝度を求める
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
//		/* 最大のbounce数を評価*/
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
//		const Vec orienting_normal = Dot(normal, ray.dir) < 0.0 ? normal : (-1.0 * normal); // 交差位置の法線（物体からのレイの入出を考慮）
//		const Vec hitpoint = ray.org + (t - EPS) * ray.dir;
//
//		// 最低バウンスを評価一定以上レイを追跡したらロシアンルーレットを実行し追跡を打ち切るかどうかを判断する
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
//		// レイと物体の交差点からの放射輝度伝達を計算
//		switch (obj.mat.type) {
//		case DIFFUSE: {
//			// 直接光のサンプリングを行う
//			if (!obj.mat.isLight()) {
////				const int shadow_ray = 1;
////				Vec direct_light;
////				for (int i = 0; i < shadow_ray; i++) {
////					direct_light = direct_light + direct_radiance_sample(hitpoint, orienting_normal, &obj, rng.next01(), rng.next01(), rng.next01()) / shadow_ray;
////				}
//
//				// orienting_normalの方向を基準とした正規直交基底(w, u, v)を作る。この基底に対する半球内で次のレイを飛ばす。
//				Vec w, u, v;
//				w = orienting_normal;
//				if (std::abs(w.x) > 0.1)
//					u = Normalize(Cross(Vec(0.0, 1.0, 0.0), w));
//				else
//					u = Normalize(Cross(Vec(1.0, 0.0, 0.0), w));
//				v = Cross(w, u);
//
//				// コサイン項を使った重点的サンプリング
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
//			//	// 完全鏡面なのでレイの反射方向は決定的。
//			//	// ロシアンルーレットの確率で除算するのは上と同じ。
//			//	//Intersection lintersect;//反射光の情報
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
//			//	// 反射方向からの直接光サンプリングする
//			//	Intersection llintersect;
//			//	TRIANGLE* lHitTri = nullptr;
//			//	lHitTri = Intersect(nodes, 0, reflection_ray, &llintersect);
//			//	//intersect_scene_triangle(reflection_ray, &lintersect);
//			//	Vec direct_light;
//			//	if (llintersect.obj_id == LightID) {
//			//		direct_light = Multiply(HitTri->mat.ref, triangles[LightID][0].mat.Le);
//			//	}
//			//	bool into = Dot(normal, orienting_normal) > 0.0; // レイがオブジェクトから出るのか、入るのか、出るならばfalse
//			//													 // Snellの法則
//			//	const double nc = 1.0; // 真空の屈折率
//			//	const double nt = 1.5; // オブジェクトの屈折率
//			//	const double nnt = into ? nc / nt : nt / nc;
//			//	const double ddn = Dot(ray.dir, orienting_normal);
//			//	const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
//			//	if (cos2t < 0.0) { // 全反射した
//			//		return direct_light + Multiply(transmittance_ratio, Multiply(obj.mat.ref, (radiance(reflection_ray, medium, rng, depth + 1, maxDepth)))) / (1.0 - scattering_probability) / russian_roulette_probability;
//			//	}
//			//	// 屈折していく方向
//			//	Vec tdir = Normalize(ray.dir * nnt - normal * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t)));
//			//	// SchlickによるFresnelの反射係数の近似
//			//	const double a = nt - nc, b = nt + nc;
//			//	const double R0 = (a * a) / (b * b);
//			//	const double c = 1.0 - (into ? -ddn : Dot(tdir, normal));
//			//	const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
//			//	const double Tr = 1.0 - Re; // 屈折光の運ぶ光の量
//			//	const double probability = 0.25 + 0.5 * Re;
//			//	// 屈折方向からの直接光サンプリングする
//			//	Ray refraction_ray = Ray(hitpoint + 2 * EPS*ray.dir, tdir);
//			//	Intersection lintersect;
//			//	lHitTri = nullptr;
//			//	lHitTri = Intersect(nodes, 0, refraction_ray, &lintersect);
//			//	Vec direct_light_refraction;
//			//	if (lintersect.obj_id == LightID) {
//			//		direct_light_refraction = Multiply(HitTri->mat.ref, triangles[LightID][0].mat.Le);
//			//	}
//			//	// 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する。（さもないと指数的にレイが増える）
//			//	// ロシアンルーレットで決定する。
//			//	if (depth > 4) {
//			//		if (rng.next01() < probability) { // 反射
//			//			return direct_light +
//			//				Multiply(transmittance_ratio, Multiply(obj.mat.ref, radiance(reflection_ray, medium, rng, depth + 1, maxDepth) * Re))
//			//				/ probability
//			//				/ (1.0 - scattering_probability)
//			//				/ russian_roulette_probability;
//			//		}
//			//		else { // 屈折
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