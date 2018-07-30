#include "bvh.h"

float T_tri = 1.0;  // �K��				
float T_aabb = 1.0;  // �K��	

float surfaceArea(const float bbox[2][3]) {
	float dx = bbox[1][0] - bbox[0][0];
	float dy = bbox[1][1] - bbox[0][1];
	float dz = bbox[1][2] - bbox[0][2];
	return 2 * ((dx*dy) + (dx*dz) + (dy*dz));
}

void  emptyAABB(float bbox[2][3]) {
	bbox[0][0] = bbox[0][1] = bbox[0][2] = INF;
	bbox[1][0] = bbox[1][1] = bbox[1][2] = -INF;
}

void mergeAABB(const float bbox1[2][3], const float bbox2[2][3], float result[2][3]) {
	for (int j = 0; j<3; j++) {
		result[0][j] = std::min(bbox1[0][j], bbox2[0][j]);
		result[1][j] = std::max(bbox1[1][j], bbox2[1][j]);
	}
}

// �|���S�����X�g����AABB�𐶐�
void creatAABBfromTriangles(const std::vector<std::shared_ptr <Triangle>> &triangles,
	float bbox[2][3]) {
	emptyAABB(bbox);

	std::for_each(triangles.begin(), triangles.end(), [&bbox](const std::shared_ptr <Triangle> t) {
		mergeAABB(t->bbox, bbox, bbox);
	});
}

void makeLeaf(std::vector<std::shared_ptr <Triangle>> &polygons, BVH_node *node) {
	node->children[0] = node->children[1] = -1;
	node->polygons = polygons;
}

// �^����ꂽ�|���S�����X�g�ɂ��āASAH �Ɋ�Â��ė̈敪��
// nodeIndex �͑Ώۃm�[�h�̃C���f�b�N�X
void constructBVH_internal(std::vector<std::shared_ptr<Triangle>> &polygons, 
	BVH_node *nodes, int used_node_count, int nodeIndex) {
	BVH_node *node = &nodes[nodeIndex];

	creatAABBfromTriangles(polygons, node->bbox);  // �S�̂��͂�AABB���v�Z


												   // �̈敪���������Apolygons ���܂ޗt�m�[�h���\�z����ꍇ���b��� bestCost �ɂ���
	float bestCost = T_tri * polygons.size();

	int bestAxis = -1;  // �����ɍł��ǂ��� (0:x, 1:y, 2:z)
	int bestSplitIndex = -1;  // �ł��ǂ������ꏊ
	float SA_root = surfaceArea(node->bbox); // �m�[�h�S�̂�AABB�̕\�ʐ�
	for (int axis = 0; axis<3; axis++) {
		// �|���S�����X�g���A���ꂼ���AABB�̒��S���W���g���Aaxis �Ń\�[�g����
		std::sort(polygons.begin(), polygons.end(),
			[axis](const std::shared_ptr <Triangle> a, const std::shared_ptr <Triangle> b) {
			double A = ((a->bbox[1][axis] + a->bbox[0][axis]) / 2);
			double B = ((b->bbox[1][axis] + b->bbox[0][axis]) / 2);

			return (A<B);//AABB �̒��S���W�Ń\�[�gx<y<z�̗D�揇�ʁH
		});

		std::vector<std::shared_ptr <Triangle>> s1, s2(polygons);  // �������ꂽ2�̗̈�
		float s1bbox[2][3]; emptyAABB(s1bbox); // S1��AABB

											   // AABB�̕\�ʐσ��X�g�Bs1SA[i], s2SA[i] �́A
											   // �uS1����i�AS2����(polygons.size()-i)�|���S��������悤�ɕ����v�����Ƃ��̕\�ʐ�
		std::vector<float> s1SA(polygons.size() + 1, INF), s2SA(polygons.size() + 1, INF);

		// �\�ȕ������@�ɂ��āAs1���� AABB �̕\�ʐς��v�Z
		for (int i = 0; i <= polygons.size(); i++) {
			s1SA[i] = fabs(surfaceArea(s1bbox)); // ���݂�S1��AABB�̕\�ʐς��v�Z
			if (s2.size() > 0) {
				// s2���ŁAaxis �ɂ��čō� (�ŏ��ʒu) �ɂ���|���S����S1�̍ŉE (�ő�ʒu) �Ɉڂ�
				std::shared_ptr <Triangle> p = s2.front();
				s1.push_back(p); s2.erase(s2.begin());//TODO�����������璼��
													  // �ڂ����|���S����AABB���}�[�W����S1��AABB�Ƃ���
				mergeAABB(s1bbox, p->bbox, s1bbox);
			}
		}

		// �t��S2����AABB�̕\�ʐς��v�Z���ASAH ���v�Z
		float s2bbox[2][3]; emptyAABB(s2bbox);
		for (int i = polygons.size(); i >= 0; i--) {
			s2SA[i] = fabs(surfaceArea(s2bbox)); // ���݂�S2��AABB�̕\�ʐς��v�Z
			if (s1.size() > 0 && s2.size() > 0) {//s1size�݂̂���s2size��-�ɂȂ��Ă��܂��
												 // SAH-based cost �̌v�Z
				float cost =
					2 * T_aabb + (s1SA[i] * s1.size() + s2SA[i] * s2.size()) * T_tri / SA_root;
				// �ŗǃR�X�g���X�V���ꂽ���H
				if (cost < bestCost) {
					bestCost = cost; bestAxis = axis; bestSplitIndex = i;
				}
			}

			if (s1.size() > 0) {
				// S1���ŁAaxis �ɂ��čŉE�ɂ���|���S����S2�̍ō��Ɉڂ�
				std::shared_ptr <Triangle> p = s1.back();
				s2.insert(s2.begin(), p); s1.pop_back();
				// �ڂ����|���S����AABB���}�[�W����S2��AABB�Ƃ���
				mergeAABB(s2bbox, p->bbox, s2bbox);
			}
		}
	}

	if (bestAxis == -1) {
		// ���݂̃m�[�h��t�m�[�h�Ƃ���̂��ł��������ǂ����ʂɂȂ���
		// => �t�m�[�h�̍쐬
		makeLeaf(polygons, node);
	}
	else {
		// bestAxis �Ɋ�Â��A���E�ɕ���
		// bestAxis �Ń\�[�g
		std::sort(polygons.begin(), polygons.end(),
			[bestAxis](const std::shared_ptr <Triangle> a, const std::shared_ptr <Triangle> b) {
			return ((a->bbox[1][bestAxis] + a->bbox[0][bestAxis]) / 2) <((b->bbox[1][bestAxis] + b->bbox[0][bestAxis]) / 2);
		});
		// ���E�̎q�m�[�h���쐬
		used_node_count++;
		node->children[0] = used_node_count;
		used_node_count++;
		node->children[1] = used_node_count;
		// �|���S�����X�g�𕪊�
		std::vector<std::shared_ptr <Triangle>> left(polygons.begin(), polygons.begin() + bestSplitIndex);
		std::vector<std::shared_ptr <Triangle>> right(polygons.begin() + bestSplitIndex, polygons.end());
		// �ċA����
		constructBVH_internal(left, nodes, used_node_count, node->children[0]);
		constructBVH_internal(right, nodes, used_node_count, node->children[1]);
	}
}

// �t�����g�G���h�֐��D������Ăׂ� nodes[0] �����[�g�Ƃ��� BVH ���\�z�����
void constructBVH(std::vector<std::shared_ptr<Triangle>> &polygons,
	BVH_node *nodes, int used_node_count) {

	//�|���S�������ő��2^()
	used_node_count = 0;
	constructBVH_internal(polygons, nodes, used_node_count, 0);  // nodes[0] �����[�g�m�[�h�Ƃ݂Ȃ�
}