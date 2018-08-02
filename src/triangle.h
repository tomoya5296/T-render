#pragma once
#include <algorithm>
#include <vector>
#include <memory>
#include "vec.h"
#include "geometry.h"
#include "ray.h"
#include "material.h"

struct TriangleMesh {
public:
	TriangleMesh(const std::vector<Vec> &ps, const std::vector<Vec> &ns);

	//TriangleMesh Data
	//std::vector<int> vertexIndices;
	std::unique_ptr<Vec[]> p;
	std::unique_ptr<Vec[]> n;
	//std::unique_ptr<Vec> s;
	//std::unique_ptr<Vec> uv;
	//std::shared_ptr<Texture<Float>> alphaMask, shadowAlphaMask;
	//std::vector<int> faceIndices;

};

class Triangle {

public:
	Triangle(const std::vector<Vec> &ps, const std::vector<Vec> &ns,
			 const Material &mat_);
	//Triangle Data
	TriangleMesh mesh;
	Vec normal;
	float bbox[2][3];
	Material mat;
};