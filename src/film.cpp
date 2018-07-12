#include "film.h"

Film::Film(const int width_, const int height_, const std::string outputfilename_):
	width(width_), height(height_), outputfilename(outputfilename_), camera()  {
	pixels = std::unique_ptr<Color[]>(new Color[width * height]);
	cx = Vec(width * 0.5135 / height, 0.0, 0.0);

	cy = Normalize(Vec(Cross(cx, camera.dir))) * 0.5135;
}

inline double Film::clamp(const double x) {
	if (x < 0.0)
		return 0.0;
	if (x > 1.0)
		return 1.0;
	return x;
}

inline int Film::to_int(const double x) {
	return int(pow(clamp(x), 1 / 2.2) * 255 + 0.5);
}

void Film::save_ppm_file() {
	std::ofstream writer(outputfilename, std::ios::out);
	writer << "P3" << std::endl;
	writer << width << " " << height << std::endl;
	writer << 255 << std::endl;
	for (int i = 0; i < width * height; i++) {
		const int r = to_int(pixels[i].x);
		const int g = to_int(pixels[i].y);
		const int b = to_int(pixels[i].z);
		writer << r << " " << g << " " << b << " ";
	}
	writer.close();
}