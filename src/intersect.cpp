#include "intersect.h"
#include "ray.h"
#include "triangle.h"

bool IntersectAABB(float aabb[2][3], const Ray &ray) {
	double ray_dir[3], ray_org[3];

	for (int i = 0; i < 3; i++) {
		if (i == 0) {
			ray_dir[i] = ray.dir.x;
			ray_org[i] = ray.org.x;
		}
		else if (i == 1) {
			ray_dir[i] = ray.dir.y;
			ray_org[i] = ray.org.y;
		}
		else {
			ray_dir[i] = ray.dir.z;
			ray_org[i] = ray.org.z;
		}
	}

	double t_max = INF;
	double t_min = -INF;

	for (int i = 0; i < 3; i++)
	{

		double t1 = (aabb[0][i] - ray_org[i]) / (ray_dir[i] + EPS);
		double t2 = (aabb[1][i] - ray_org[i]) / (ray_dir[i] + EPS);
		double t_far = std::max(t1, t2);
		double t_near = std::min(t1, t2);
		t_max = std::min(t_max, t_far);
		t_min = std::max(t_min, t_near);
		if (t_min > t_max) { return false; }
	}
	return true;
};

bool intersect(const Ray &ray, 
			   const std::shared_ptr<Triangle> tri,
			   Hitpoint *hitpoint) {
	//Tomas Moller method
	Vec edge2 = tri->mesh.p[1] - tri->mesh.p[0];
	Vec	edge1 = tri->mesh.p[2] - tri->mesh.p[0];
	Vec invRay = -1 * ray.dir;
	double denominator = det(edge1, edge2, invRay);

	if (denominator < 0.0) {
		Vec temp = edge1;
		edge1 = edge2;
		edge2 = temp;
	}
	else if (denominator == 0.0) {
		return false;
	}
	denominator = det(edge1, edge2, invRay);
	if (denominator <= 0.0) {
		return false;
	}

	Vec d = ray.org - tri->mesh.p[0];
	double u = det(d, edge2, invRay) / denominator;

	if ((u >= 0.0) && (u <= 1.0)) {
		double v = det(edge1, d, invRay) / denominator;
		if ((v >= 0.0) && (u + v <= 1.0)) {
			double t1 = det(edge1, edge2, d) / denominator;
			if (t1 < 0.0) {
				return false;
			}
			hitpoint->distance = t1;
			hitpoint->position = ray.org + hitpoint->distance*ray.dir;
			hitpoint->normal = tri->normal;
			return true;
		}
	}
	return false;
}

std::shared_ptr<Triangle> intersect(BVH_node *nodes, int index, const Ray &ray, Intersection *intersection) {
	// AABB とレイの交差判定
	if (IntersectAABB(nodes[index].bbox, ray)) {
		// 交差している

		// 中間ノードか？
		if (nodes[index].children[0] != -1) {
			// 中間ノード
			// 両方の子ノードについて調べる
			std::shared_ptr<Triangle> childResult = nullptr;
			for (int i = 0; i < 2; i++) {
				std::shared_ptr<Triangle> result = intersect(nodes, nodes[index].children[i], ray, intersection);
				if (result != nullptr) {
					childResult = result;
				}
			}
			if (childResult != nullptr) return childResult;
		}
		else {
			// 葉ノード
			std::shared_ptr<Triangle> result = nullptr;
			for (std::shared_ptr<Triangle> tri : nodes[index].polygons) {
				// ポリゴンとレイの交差判定
				// distance に交差していた場合のレイからの距離、normal にポリゴンの法線が入る
				Hitpoint hitopoint;
				if (intersect(ray, tri, &hitopoint)) {
					// 既に交差したと判定された他のポリゴンより、レイの始点に近いかどうか
					if (hitopoint.distance<intersection->hitpoint.distance) {
						result = tri;
						intersection->hitpoint = hitopoint;
						//intersect->Mat = tri->mat;
						//intersect->obj_id = tri->obj_id;
					}
				}
			}
			if (result != nullptr) return result;
		}
	}
	else {
		return nullptr;// 交差していない (何もする必要なし)
	}
	return nullptr;
}