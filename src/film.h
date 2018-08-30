#pragma once
#include <fstream>
#include <string>
#include <memory>
#include <vector>
#include "vec.h"
#include "camera.h"

class Film
{
public:
	Film(const int width_, const int height_,
		 const std::string outputfilename_);
	void save_ppm_file(const int itere_num);
	int get_width() const ;
	int get_height() const ;

private:
	inline double clamp(const double x);
	inline int to_int(double x);

	const int width;
	const int height;

public:
	const Camera camera;
	Vec cx, cy;
	std::unique_ptr<Color[]> pixels;
	const std::string outputfilename;
};

