#include "ppm.h"

P_Photon_map::P_Photon_map()
{
	hash_s = 0.0;
	maxphotn_num = 100000000;
}

P_Photon_map::~P_Photon_map()
{
}


inline unsigned int P_Photon_map::hash(const int ix, const int iy, const int iz) {

	return (unsigned)((ix * 73856093) ^ (iy * 19349663) ^ (iz * 83492791)) % hash_grid.size();
}

void P_Photon_map::build_hash_grid(const int w, const int h) {

	//heuristic for initial radius
	Vec size = hpbbox.max - hpbbox.min;
	float irad = ((size.x + size.y + size.z) / 3.0) / ((w + h) / 2.0) * 2.0;

	//determine hash table size
	//we now find the bounding box of all measurement points inflated by the initial radius
	hpbbox.reset();
	int photon_count = 0;
	for (auto itr = hitpoints.begin(); itr != hitpoints.end(); itr++) {
		auto hp = (*itr);
		hp->r2 = irad *irad;
		hp->n = 0;
		hp->flux = Vec(0.0);
		photon_count++;
		hpbbox.merge(hp->pos - Vec(irad));
		hpbbox.merge(hp->pos + Vec(irad));
	}

	//make each grid cell two times larger than the initial radius
	hash_s = 1.0 / (irad * 2.0);

	//build the hash table
	hash_grid.resize(photon_count);
	hash_grid.shrink_to_fit();
	for (auto itr = hitpoints.begin(); itr != hitpoints.end(); ++itr) {
		auto hp = (*itr);
		auto min = ((hp->pos - Vec(irad)) - hpbbox.min) * hash_s;
		auto max = ((hp->pos + Vec(irad)) - hpbbox.min) * hash_s;

		for (int iz = abs(int(min.z)); iz <= abs(int(max.z)); iz++)
		{
			for (int iy = abs(int(min.y)); iy <= abs(int(max.y)); iy++)
			{
				for (int ix = abs(int(min.x)); ix <= abs(int(max.x)); ix++)
				{
					int hv = hash(ix, iy, iz);
					hash_grid[hv].push_back(hp);
				}
			}
		}
	}
}

void P_Photon_map::trace_ray(Scene &scene, const Film &film) {
	const int width = film.get_width();
	const int height = film.get_height();
	//parallel_for(0, width * height, [&](int i) {//これやるとバグる・・
	for (int i = 0; i < width * height; i++) {
		// 乱数
		Random rng(i);
		// テントフィルターによってサンプリング
		const double r1 = 2.0 * rng.next01();
		const double r2 = 2.0 * rng.next01();
		const double dx = r1 < 1.0 ? sqrt(r1) - 1.0 : 1.0 - sqrt(2.0 - r1);
		const double dy = r2 < 1.0 ? sqrt(r2) - 1.0 : 1.0 - sqrt(2.0 - r2);
		const double px = ((i % width) + dx + 0.5) / width - 0.5;
		const double py = ((height - (i / width) - 1) + dy + 0.5) / height - 0.5;

		const Vec dir = film.cx * px + film.cy * py + film.camera.dir;
		Ray ray(film.camera.pos + dir * 13.0, Normalize(dir));
		trace(scene, ray, 0, true, Vec(0.0), Vec(1.0), i, 10, film);
		//});
	}
	build_hash_grid(width, height);
}

void P_Photon_map::trace(Scene &scene, const Ray &ray, int depth, const bool isEye,
	const Vec &fl, const Vec &adj, int i, const int maxdepth, const Film &film) {

	double t;
	int id;

	depth++;
	std::shared_ptr<Triangle> HitTri = nullptr;
	Intersection intersection;
	HitTri = intersect(&scene.nodes[0], 0, ray, &intersection);

	if (HitTri == nullptr || depth > maxdepth) {
		return;
	}

	const Triangle &obj = *HitTri;
	const Vec f = obj.mat.ref;
	const Vec normal = obj.normal;
	const Vec orienting_normal = Dot(normal, ray.dir) < 0.0 ? normal : (-1.0 * normal);
	const Vec hitpos = ray.org + (intersection.hitpoint.distance - EPS) * ray.dir;

	if (HitTri->mat.type == DIFFUSE) {
		if (isEye) {
			//eye ray
			//store the measurement point
			auto hp = new Hitrecord;
			hp->f = f * adj;
			hp->pos = hitpos;
			hp->normal = orienting_normal;
			hp->flux = Vec(0.0);
			hp->r2 = 0.0;
			hp->n = 0;
			hp->idx = i;
			hitpoints.push_back(hp);
			//std::cout << hitpoints.size() << std::endl;
			//find the bounding box of all the measurement points
			hpbbox.merge(hitpos);
		}
		else {
			// photon ray
			// find neighboring measurement points and accumulate flux via progressive density estimation
			auto hh = (hitpos - hpbbox.min) * hash_s;
			auto ix = abs(int(hh.x));
			auto iy = abs(int(hh.y));
			auto iz = abs(int(hh.z));
			// strictly speaking, we should use #pragma omp critical here.
			// it usually works without an artifact due to the fact that photons are 
			// rarely accumulated to the same measurement points at the same time (especially with QMC).
			// it is also significantly faster.
			{
				auto list = hash_grid[hash(ix, iy, iz)];
				for (auto itr = list.begin(); itr != list.end(); itr++)
				{
					auto hp = (*itr);
					auto v = hp->pos - hitpos;
					// check normals to be closer than 90 degree (avoids some edge brightning)
					if ((Dot(hp->normal, normal) > 1e-3) && (Dot(v, v) <= hp->r2))
					{
						// unlike N in the paper, hp->n stores "N / ALPHA" to make it an integer value
						auto g = (hp->n * ALPHA + ALPHA) / (hp->n * ALPHA + 1.0);
						hp->r2 = hp->r2 * g;
						hp->n++;
						hp->flux = (hp->flux + (hp->f * fl) / PI) * g;
					}
				}
			}
		}
	}
	else if (obj.mat.type == SPECULAR) {
		trace(scene, Ray(hitpos, reflect(ray.dir, normal)), depth,
			isEye, f * fl, f * adj, i, maxdepth, film);
	}
	else if (obj.mat.type == REFRACTION) {
		Ray lr(hitpos - ray.dir * EPS, reflect(ray.dir, normal));
		const bool into = Dot(normal, orienting_normal) > 0.0;
		const float nc = 1.0;
		const float nt = 1.3;
		const float nnt = (into) ? nc / nt : nt / nc;
		const float ddn = Dot(ray.dir, orienting_normal);
		const float cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

		// total internal reflection
		if (cos2t < 0) {
			return trace(scene, lr, depth, isEye, f * fl, f * adj,
				i, maxdepth, film);
		}

		const Vec td = Normalize(ray.dir * nnt - normal * ((into ? 1 : -1) * (ddn * nnt + sqrt(cos2t))));
		const float a = nt - nc;
		const float b = nt + nc;
		const float R0 = a * a / (b * b);
		const float c = 1.0 - (into ? -ddn : Dot(td, -1.0 * orienting_normal));
		const float Re = R0 + (1 - R0) * c * c * c * c * c;
		const float P = Re * 0.5 + 0.25;
		//const float nnt2 = pow(into ? nc / nt : nt / nc, 2.0);
		//const float Tr = (1.0 - Re) * nnt2;
		const Ray  rr(hitpos + ray.dir * EPS, td);
		const Vec fa = f * adj;
		const Vec ffl = f * fl;
		if (isEye)
		{
			// eye ray (trace both rays)
			trace(scene, lr, depth, isEye, ffl, fa * Re, i, maxdepth, film);
			trace(scene, rr, depth, isEye, ffl, fa * (1.0 - Re), i, maxdepth, film);
		}
		else
		{
			Random rng(i);
			// photon ray (pick one via Russian roulette)
			if (rng.next01() < P) {
				trace(scene, lr, depth, isEye, ffl, fa * Re, i, maxdepth, film);
			}
			else {
				trace(scene, rr, depth, isEye, ffl, fa * (1.0 - Re), i, maxdepth, film);
			}
		}
	}
}

Ray P_Photon_map :: genp(Vec* f, int i, Random rng)
{
	// generate a photon ray from the point light source with QMC

	(*f) = Vec(100500, 100500, 100500) * (PI * 4.0); // flux
	const double r1 = 2.0 * PI * rng.next01();
	const double r2 = rng.next01();
	const double r2s = sqrt(r2);
	Vec dir = Normalize((Vec(1.0, 0.0, 0.0) * cos(r1) * r2s
		+ Vec(0.0, 0.0, 1.0) * sin(r1) * r2s
		+ Vec(0.0, -1.0, 0.0) * sqrt(1.0 - r2)));

	return Ray(Vec(0.0, 20.0, 20.0), dir);
}

void P_Photon_map::photon_trace(Scene &scene, const Film &film) {

	Vec vw = Vec(1.0, 1.0, 1.0);

	parallel_for(0, maxphotn_num, [&](int i) {
		Random rng(i);
		int m = 1000 * i;
		Vec f(0.0);
		Ray ray = genp(&f, m, rng);
		trace(scene, ray, 0, false, f, vw, i, 10, film);
	});
}

void P_Photon_map :: density_estimation(Film *film)
{
	// density estimation
	for (auto itr = hitpoints.begin(); itr != hitpoints.end(); itr++)
	{
		auto hp = (*itr);
		auto i = hp->idx;
		film->pixels[i] = film->pixels[i] + hp->flux * (1.0 / (PI * hp->r2 * maxphotn_num * 1000.0));
	}
}