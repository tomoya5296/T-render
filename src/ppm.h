#pragma once
#include <list>
#include "ray.h"
#include "triangle.h"
#include "intersect.h"
#include "parallel.h"
#include "random.h"
#include "vec.h"
#include "film.h"
#include "scene.h"

struct Hitrecord
{
	Vec pos;
	Vec normal;
	Vec flux;
	Vec f;
	double r2;
	unsigned int n;
	int idx;
};


struct BoundingBox
{
	Vec min;
	Vec max;

	inline void merge(const Vec &v) {
		min.x = (v.x < min.x) ? v.x : min.x;
		min.y = (v.y < min.y) ? v.y : min.y;
		min.z = (v.z < min.z) ? v.z : min.z;

		max.x = (v.x > max.x) ? v.x : max.x;
		max.y = (v.y > max.y) ? v.y : max.y;
		max.z = (v.z > max.z) ? v.z : max.z;
	}

	inline void reset() {
		min = Vec( INF,  INF,  INF);
		max = Vec(-INF, -INF, -INF);
	}
};



class P_Photon_map
{
public:
	P_Photon_map();
	~P_Photon_map();
	void trace_ray(Scene &scene, const Film &film);
	void photon_trace(Scene &scene, const Film &film);
	void density_estimation(Film *film);

private:
	void trace(Scene &scene, const Ray &ray, int depth, const bool isEye,
		const Vec &fl, const Vec &adj, int i, const int maxdepth, const Film &film);
	void build_hash_grid(const int w, const int h);
	inline unsigned int hash(const int ix, const int iy, const int iz);
	Ray genp(Vec* f, int i, Random rng);

private:
	std::list<Hitrecord*> hitpoints;
	std::vector<std::list<Hitrecord*>> hash_grid;
	BoundingBox hpbbox;
	double hash_s;
	int maxphotn_num;

};

