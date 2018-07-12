#pragma once
#define TINYOBJLOADER_IMPLEMENTATION
#include <vector>
#include <memory>
#include "dir.h"
#include "sphere.h"
#include "triangle.h"
#include "parallel.h"
#include"tiny_obj_loader.h"


const Sphere spheres[] = {
	Sphere(16.5,Vec(73,30.5,78), Color(0.75, 0.75, 0.75))// ��
};

std::vector<std::string> objList =
{
	std::string(SCENE_DIRECTORY) + "bunny.obj"
};

void objectload(std::vector<std::shared_ptr<Triangle>> *tris,
	const std::vector<std::string> &strList) {
	tris->resize(0);

	parallel_for(0, strList.size(), [&](int i){
		std::string inputfile = strList[i];
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;
		bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

		if (!err.empty()) { // `err` may contain warning message.
			std::cerr << err << std::endl;
		}

		if (!ret) {
			exit(1);
		}

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			int  face_number = shapes[s].mesh.num_face_vertices.size();
			tris->reserve(tris->size() + face_number);
			size_t index_offset = 0;

			std::vector<Vec>  vertex(3);
			std::vector<Vec>  normal(3);
			for (size_t f = 0; f < face_number; f++) {
				int fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t ve = 0; ve < fv; ve++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + ve];
					float vx = attrib.vertices[3 * idx.vertex_index + 0];
					float vy = attrib.vertices[3 * idx.vertex_index + 1];
					float vz = attrib.vertices[3 * idx.vertex_index + 2];
					float nx = attrib.normals[3 * idx.normal_index + 0];
					float ny = attrib.normals[3 * idx.normal_index + 1];
					float nz = attrib.normals[3 * idx.normal_index + 2];
					/*float tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					float ty = attrib.texcoords[2 * idx.texcoord_index + 1];*/
					vertex[ve] = Vec(vx, vy, vz);
					normal[ve] = Vec(nx, ny, nz);
				}
				tris->push_back(std::make_shared<Triangle>(vertex, normal));
				index_offset += fv;
				// per-face material
				shapes[s].mesh.material_ids[f];
			}
		}
	});
}

