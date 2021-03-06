#include "scene.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include"tiny_obj_loader.h"

Scene::Scene(const std::vector<Object>& objList)
{
	//load mesh
	objectload(objList);

	//make bvh
	nodes.resize(100000);
	int used_node_count = 0;
	constructBVH(tris, &nodes[0], used_node_count);
	Lnodes.resize(10000);
	used_node_count = 0;
	constructBVH(Ltris, &Lnodes[0], used_node_count);
}

void Scene::objectload(const std::vector<Object> &objList) {
	tris.reserve(100000);
	Ltris.reserve(10000);

	//parallel_for(0, objList.size(), [&](int i) {
	for (int i = 0; i < objList.size(); i++) {
		std::string inputfile = objList[i].filename;
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
				std::shared_ptr<Triangle> ptri = std::make_shared<Triangle>(vertex, normal, objList[i].mat);
				tris.push_back(ptri);
				if (ptri->mat.isLight()) {
					Ltris.push_back(ptri);
				}

				index_offset += fv;
				// per-face material
				shapes[s].mesh.material_ids[f];
			}
		}
		std::cout << objList[i].filename << "が読み込まれました\n" << std::endl;
		//});
	}
}