#include <fstream>
#include <memory>
#include "vec.h"
#include "ray.h"
#include "film.h"
#include "random.h"
#include "scene.h"
#include "parallel.h"
#include "intersect.h"
#include "bvh.h"


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

	//bvh datas

	std::vector<std::shared_ptr<Triangle>> tris;
	objectload(&tris, objList);
	static BVH_node nodes[10000];  // ノードリスト．本当は適切なノード数を確保すべき
	int used_node_count = 0;  // 現在使用されているノードの数
	constructBVH(tris, nodes, used_node_count);

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
			std::shared_ptr<Triangle> hittri = nullptr;
			Intersection *tempintersect = nullptr;
			hittri = intersect(nodes, 0, ray, tempintersect);
			film.pixels[i] = Vec(1.0 / tempintersect->hitpoint.distance, 0.0, 0.0);
		}
	});

	film.save_ppm_file();
}