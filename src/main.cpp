#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <memory>
#include <chrono>
#include "vec.h"
#include "ray.h"
#include "film.h"
#include "random.h"
#include "scene.h"
#include "parallel.h"
#include "intersect.h"
#include "bvh.h"
#include "radiance.h"
#include "pm.h"
#include "ppm.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi_image_write.h"
#include "stbi_image.h"



void main(int argc, char **argv) {
	auto start = std::chrono::system_clock::now();

	const int constant_num = 4;
	int width = 1920 / constant_num, height = 1080 / constant_num,
		spp = 32, maxdepth = 20;

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

	std::vector<Object> objList;
	objList.push_back(Object(std::string(SCENE_DIRECTORY) + "light_plane_up.obj",
		Color(5.0), Color(0.0), DIFFUSE));
	objList.push_back(Object(std::string(SCENE_DIRECTORY) + "light_plane_far.obj",
		Color(5.0), Color(0.0), DIFFUSE));
	//objList.push_back(Object(std::string(SCENE_DIRECTORY) + "cup.obj",
	//	Color(0.0), Color(1.0, 1.0, 1.0), REFRACTION));
	//objList.push_back(Object(std::string(SCENE_DIRECTORY) + "ice.obj",
	//	Color(0.0), Color(1.0, 1.0, 1.0), REFRACTION));
	//objList.push_back(Object(std::string(SCENE_DIRECTORY) + "lucy.obj",
	//	Color(0.0), Color(1.0, 1.0, 0.0), DIFFUSE));
	//objList.push_back(Object(std::string(SCENE_DIRECTORY) + "rt6.obj",
	//	Color(0.0), Color(0.0, 0.8, 0.8), DIFFUSE));
	//objList.push_back(Object(std::string(SCENE_DIRECTORY) + "water.obj",
	//	Color(0.0), Color(1.0, 0.0, 0.0), REFRACTION));
	objList.push_back(Object(std::string(SCENE_DIRECTORY) + "bunny.obj",
		Color(0.0), Color(1.0, 0.0, 0.0), DIFFUSE));
	objList.push_back(Object(std::string(SCENE_DIRECTORY) + "floor.obj",
		Color(0.0), Color(1.0, 1.0, 1.0), SPECULAR));
	Scene scene(objList);
	//P_Photon_map ppm;
	//ppm.trace_ray(scene, film);
	//ppm.photon_trace(scene, film);
	//ppm.density_estimation(&film);

	std::cout << "bvh built" << std::endl;
	//parallel_for(0, width * height, [&](int i) {
		//for (int s = 0; s < spp; s++) {
	parallel_for(0, spp, [&](int s) {
		for (int i = 0; i < width * height; i++) {
			// ����
			Random rng(i * s);

			// �e���g�t�B���^�[�ɂ���ăT���v�����O
			const double r1 = 2.0 * rng.next01();
			const double r2 = 2.0 * rng.next01();
			const double dx = r1 < 1.0 ? sqrt(r1) - 1.0 : 1.0 - sqrt(2.0 - r1);
			const double dy = r2 < 1.0 ? sqrt(r2) - 1.0 : 1.0 - sqrt(2.0 - r2);
			const double px = ((i % width) + dx + 0.5) / width - 0.5;
			const double py = ((height - (i / width) - 1) + dy + 0.5) / height - 0.5;
			// ���ˋP�x�̌v�Z
			const Vec dir = film.cx * px + film.cy * py + film.camera.dir;
			Ray ray(film.camera.pos + dir * 18.0, Normalize(dir));
			const Color L = radiance(scene, ray, rng, maxdepth);
			Assertion(L.isValid(), "Radiance is invalid: (%f, %f %f)", L.x, L.y, L.z);
			film.pixels[i] = (film.pixels[i] * (float)(spp - 1.0) + L) / spp;
		}
		/*if(i % width == 0)
			printf(" %f finsed\n", ((float)i / ((float)width * (float)height)) * 100.0);*/
	});
	film.save_ppm_file();
	auto end = std::chrono::system_clock::now();
	auto dif = end - start;
	fprintf(stdout, "Ray Tracing : %lld(msec)\n", std::chrono::duration_cast<std::chrono::milliseconds>(dif).count());
}