#include "triangle.h"

TriangleMesh::TriangleMesh(const std::vector<Vec> &ps, const std::vector<Vec> &ns) {
	p.reset(new Vec[3]);
	n.reset(new Vec[3]);
	for (int i = 0; i < 3; i++) {
		p[i] = ps[i];
		n[i] = ns[i];
	}
}

Triangle::Triangle(const std::vector<Vec> &ps, const std::vector<Vec> &ns)
	: mesh(ps, ns)
{
	normal = Normalize(Cross((ps[1] - ps[0]), (ps[2] - ps[0])));
	bbox[0][0] = std::min(std::min(ps[0].x, ps[1].x), ps[2].x);
	bbox[0][1] = std::min(std::min(ps[0].y, ps[1].y), ps[2].y);
	bbox[0][2] = std::min(std::min(ps[0].z, ps[1].z), ps[2].z);
	bbox[1][0] = std::max(std::max(ps[0].x, ps[1].x), ps[2].x);
	bbox[1][1] = std::max(std::max(ps[0].y, ps[1].y), ps[2].y);
	bbox[1][2] = std::max(std::max(ps[0].z, ps[1].z), ps[2].z);
};