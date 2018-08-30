#define _CRT_SECURE_NO_WARNINGS
#include "film.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi_image_write.h"
#include "stbi_image.h"

Film::Film(const int width_, const int height_, const std::string outputfilename_):
	width(width_), height(height_), outputfilename(outputfilename_), camera()  {
	pixels = std::unique_ptr<Color[]>(new Color[width * height]);
	cx = Vec(width * 0.2535 / height, 0.0, 0.0);

	cy = Normalize(Vec(Cross(cx, camera.dir))) * 0.2535;
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

void Film::save_ppm_file(const int itere_num) {
	std::vector<unsigned char> image_buffer(width * height * 4);
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int pos = (y * width + x) * 4;
			int pixel_pos = y * width + x;
			image_buffer[pos + 0] = to_int(pixels[pixel_pos].x);
			image_buffer[pos + 1] = to_int(pixels[pixel_pos].y);
			image_buffer[pos + 2] = to_int(pixels[pixel_pos].z);
			image_buffer[pos + 3] = 0xFF;
		}
	}
	std::string file_name = "output" + std::to_string(itere_num) + ".png";
	
	stbi_write_png(file_name.c_str(), width, height,
		STBI_rgb_alpha, &(*image_buffer.begin()), 0);
}

int Film::get_height() const { return height; }
int Film::get_width() const { return width; }