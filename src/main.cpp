#include <fstream>
#include <memory>

#include "vec.h"
#include "ray.h"
#include "film.h"
#include "random.h"
#include "scene.h"
#include "parallel.h"


void main(int argc, char **argv) {
	int width = 640, height = 640, spp = 1, maxdepth = 10;
	//for (int i = 1; i < argc; i++) {
	//	if (strcmp(argv[i], "--width") == 0) {
	//		width = std::atoi(argv[++i]);
	//	}
	//	if (strcmp(argv[i], "--width") == 0) {
	//		width = std::atoi(argv[++i]);
	//	}
	//	if (strcmp(argv[i], "--width") == 0) {
	//		width = std::atoi(argv[++i]);
	//	}
	//	if (strcmp(argv[i], "--width") == 0) {
	//		width = std::atoi(argv[++i]);
	//	}
	//}

	//out put image
	Film film(width, height, "../output/image.ppm");

	std::vector<std::shared_ptr<Triangle>> tris;
	objectload(&tris, objList);

	parallel_for(0, width * height, [&](int i) {
		// 乱数
		Random rng(i);
		for (int s = 0; s < spp; s++) {
			// テントフィルターによってサンプリング
			const double r1 = 2.0 * rng.next01();
			const double r2 = 2.0 * rng.next01();
			const double dx = r1 < 1.0 ? sqrt(r1) - 1.0 : 1.0 - sqrt(2.0 - r1);
			const double dy = r2 < 1.0 ? sqrt(r2) - 1.0 : 1.0 - sqrt(2.0 - r2);
			const double px = ((i % width) + dx + 0.5) / width - 0.5;
			const double py = ((height - (i / width) - 1) + dy + 0.5) / height - 0.5;

			// 放射輝度の計算
			const Vec dir = film.cx * px + film.cy * py + film.camera.dir;
			const Ray ray(film.camera.pos + dir * 13.0, Normalize(dir));
			//const Color L = radiance(ray, Medium(), rng, 0, maxDepth);
			//Assertion(L.isValid(), "Radiance is invalid: (%f, %f %f)", L.x, L.y, L.z);
			Hitpoint hitpoint;
			for (int j = 0; j < tris.size(); j++) {
				Hitpoint hitpoint_temp;
				if (tris[j]->intersect(ray, &hitpoint_temp)) {
					if (hitpoint_temp.distance < hitpoint.distance) {
						hitpoint = hitpoint_temp;
					}
				}
			}
			film.pixels[i] = Vec(1.0 / hitpoint.distance, 0.0, 0.0);
		}
	});

	film.save_ppm_file();
}