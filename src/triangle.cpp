#include "triangle.h"

TriangleMesh::TriangleMesh(const std::vector<Vec> &ps, const std::vector<Vec> &ns) {
	p.reset(new Vec[3]);
	n.reset(new Vec[3]);
	for (int i = 0; i < 3; i++) {
		p[i] = ps[i];
		n[i] = ns[i];
	}
}

bool Triangle::intersect(const Ray &ray, Hitpoint *hitpoint) {
	//Tomas Moller method
	Vec edge2 = mesh.p[1] - mesh.p[0];
	Vec	edge1 = mesh.p[2] - mesh.p[0];
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

	Vec d = ray.org - mesh.p[0];
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
			hitpoint->normal = normal;
			return true;
		}
	}
	return false;

}


Triangle::Triangle(const std::vector<Vec> &p, const std::vector<Vec> &n)
	: mesh(p, n)
{
	normal = Normalize(Cross((p[1] - p[0]), (p[2] - p[0])));
	bbox[0][0] = std::min(std::min(p[0].x, p[1].x), p[2].x);
	bbox[0][1] = std::min(std::min(p[0].y, p[1].y), p[2].y);
	bbox[0][2] = std::min(std::min(p[0].z, p[1].z), p[2].z);
	bbox[1][0] = std::max(std::min(p[0].x, p[1].x), p[2].x);
	bbox[1][1] = std::max(std::min(p[0].y, p[1].y), p[2].y);
	bbox[1][2] = std::max(std::min(p[0].z, p[1].z), p[2].z);
};