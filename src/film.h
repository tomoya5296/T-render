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
	int get_width() const ;
	int get_height() const ;

private:
	inline double clamp(const double x);
	inline int to_int(double x);

	const int width;
	const int height;
	const std::string outputfilename;

public:
	const Camera camera;
	Vec cx, cy;
	std::unique_ptr<Color[]> pixels;
};

