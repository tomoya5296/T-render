#pragma once
#include <vector>
#include <memory>
#include "dir.h"
#include "sphere.h"
#include "triangle.h"
#include "bvh.h"
#include "parallel.h"
#include "material.h"


struct Object
{
	Object(std::string filename_, Color Le_, Color ref_, RefType type_)
	: filename(filename_), mat(Le_, ref_, type_){}

public:
	std::string filename;
	Material mat;
};

class Scene
{
public:
	Scene(const std::vector<Object> &objList);

private:
	void objectload(const std::vector<Object> &objList);

public:
	std::vector<Object> ObjList;
	std::vector<std::shared_ptr<Triangle>> tris;
	std::vector<std::shared_ptr<Triangle>> Ltris;
	std::vector<BVH_node> nodes;
};

