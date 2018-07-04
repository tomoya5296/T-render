#include <fstream>
#include <memory>

#include "vec.h"
#include "ray.h"
#include "film.h"

void main(int argc, char **argv) {
	int width = 256, height = 256, spp = 64, maxdepth = 10;
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
	Film img(width, height, "../output/image.ppm");

	for (int i = 0; i < width * height; i++) {
		img.pixels[i] = Vec(1.0, 0.0, 0.0);
	}

	img.save_ppm_file();
}