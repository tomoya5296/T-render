#pragma once
#include <fstream>
#include <string>
#include <memory>
#include "vec.h"
#include "camera.h"

class Film
{
public:
	Film(const int width_, const int height_,
		 const std::string outputfilename_);
	void save_ppm_file();

private:
	inline double clamp(const double x);
	inline int to_int(double x);

	const int width;
	const int height;
	const std::string outputfilename;
	const Camera camera;
	Vec cx, cy;

public:
	std::unique_ptr<Color[]> pixels;
};

