#include "triangle.h"

TriangleMesh::TriangleMesh(const std::vector<Vec> &ps, const std::vector<Vec> &ns) {
	p.reset(new Vec[3]);
	n.reset(new Vec[3]);
	for (int i = 0; i < 3; i++) {
		p[i] = ps[i];
		n[i] = ns[i];
	}
}

TriangleMesh::TriangleMesh() {
	p.reset(new Vec[3]);
	n.reset(new Vec[3]);
	for (int i = 0; i < 3; i++) {
		p[i] = Vec(0.0);
		n[i] = Vec(0.0);
	}
}

Triangle::Triangle(const std::vector<Vec> &ps, const std::vector<Vec> &ns,
				   const Material &mat_) : mesh(ps, ns), mat(mat_)
{
	normal = Normalize(Cross((ps[1] - ps[0]), (ps[2] - ps[0])));
	bbox[0][0] = std::min(std::min(ps[0].x, ps[1].x), ps[2].x);
	bbox[0][1] = std::min(std::min(ps[0].y, ps[1].y), ps[2].y);
	bbox[0][2] = std::min(std::min(ps[0].z, ps[1].z), ps[2].z);
	bbox[1][0] = std::max(std::max(ps[0].x, ps[1].x), ps[2].x);
	bbox[1][1] = std::max(std::max(ps[0].y, ps[1].y), ps[2].y);
	bbox[1][2] = std::max(std::max(ps[0].z, ps[1].z), ps[2].z);
}
Triangle::Triangle() : mat(Color(0.0), Color(0.0), DIFFUSE), normal(Vec(0.0))
{
	const std::vector<Vec> ps(3);
	const std::vector<Vec> ns(3);
	bbox[0][0] = -INF;
	bbox[0][1] = -INF;
	bbox[0][2] = -INF;
	bbox[1][0] = INF;
	bbox[1][1] = INF;
	bbox[1][2] = INF;
	
};
