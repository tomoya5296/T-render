#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include "triangle.h"

// BVH �̃m�[�h�̍\��
struct BVH_node {
	float bbox[2][3];  // bbox[0,1]: AABB �̍ŏ�,�ő���W�D bbox[hoge][0,1,2]: x,y,z���W
	int children[2];    // �q�m�[�h
	std::vector<std::shared_ptr <Triangle>> polygons;  // �i�[����Ă���|���S�� (�t�m�[�h�̂ݗL��)
};
// AABB �̕\�ʐόv�Z
float surfaceArea(const float bbox[2][3]);

// ��� AABB ���쐬
void  emptyAABB(float bbox[2][3]);

// 2��AABB���}�[�W
void mergeAABB(const float bbox1[2][3], const float bbox2[2][3], float result[2][3]);

// �|���S�����X�g����AABB�𐶐�
void creatAABBfromTriangles(const std::vector<std::shared_ptr <Triangle>> &triangles,
	float bbox[2][3]);		

// nodeIndex �Ŏw�肳�ꂽ�m�[�h���Apolygons ���܂ޗt�m�[�h�ɂ���
void makeLeaf(std::vector<std::shared_ptr <Triangle>> &polygons, BVH_node *node);

// �^����ꂽ�|���S�����X�g�ɂ��āASAH �Ɋ�Â��ė̈敪��
// nodeIndex �͑Ώۃm�[�h�̃C���f�b�N�X
void constructBVH_internal(std::vector<std::shared_ptr<Triangle>> &polygons,
	BVH_node *nodes, int used_node_count, int nodeIndex);

// �t�����g�G���h�֐��D������Ăׂ� nodes[0] �����[�g�Ƃ��� BVH ���\�z�����
void constructBVH(std::vector<std::shared_ptr<Triangle>> &polygons,
	BVH_node *nodes, int used_node_count);
