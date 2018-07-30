#include "triangle.h"

TriangleMesh::TriangleMesh(const std::vector<Vec> &ps, const std::vector<Vec> &ns) {
	p.reset(new Vec[3]);
	n.reset(new Vec[3]);
	for (int i = 0; i < 3; i++) {
		p[i] = ps[i];
		n[i] = ns[i];
	}
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