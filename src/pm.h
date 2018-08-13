#pragma once
#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "random.h"
#include "vec.h"
#include "ray.h"
#include "triangle.h"
#include "intersect.h"
#include "scene.h"

typedef struct Photon {
	float pos[3];
	short plane;
	unsigned char theta, phi;
	float power[3];
} Photon;

typedef struct NearestPhotons {
	int max;
	int found;
	int got_heap;
	float pos[3];
	float *dist2;
	const Photon **index;
} NearestPhotons;

class Photon_map
{
public:
	Photon_map(const int max_phot);
	~Photon_map();

	void store(
		const float power[3],
		const float pos[3],
		const float dir[3]
	);

	int getMaxphotonNum() {
		return max_photons;
	}

	void scale_photon_power(const float scale);
	Color radiance(Scene &scene, const Ray ray, Random &rng);

	// balance the kd-tree
	void balance(void);

	void irradiance_estimate(
		float irrad[3],
		const float pos[3],
		const float normal[3],
		const float max_dist,
		const int nphotons) const;

	void locate_photons(
		NearestPhotons *const np,
		const int index) const;
	
	void photon_dir(
		float *dir,
		const Photon *p) const;

	void genep(Scene &scene, Random &rng);

private:

	void balance_segment(
		Photon **pbal,
		Photon **porg,
		const int index,
		const int start,
		const int end);

	void median_split(
		Photon **p,
		const int start,
		const int end,
		const int median,
		const int axis);

	Photon *photons;

	int stored_photons;
	int half_stored_photons;
	int max_photons;
	int prev_scale;

	float costheta[256];
	float sintheta[256];
	float cosphi[256];
	float sinphi[256];

	float bbox_min[3];
	float bbox_max[3];
};

