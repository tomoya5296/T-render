#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include "triangle.h"

// BVH のノードの構造
struct BVH_node {
	float bbox[2][3];  // bbox[0,1]: AABB の最小,最大座標． bbox[hoge][0,1,2]: x,y,z座標
	int children[2];    // 子ノード
	std::vector<std::shared_ptr <Triangle>> polygons;  // 格納されているポリゴン (葉ノードのみ有効)
};
// AABB の表面積計算
float surfaceArea(const float bbox[2][3]);

// 空の AABB を作成
void  emptyAABB(float bbox[2][3]);

// 2つのAABBをマージ
void mergeAABB(const float bbox1[2][3], const float bbox2[2][3], float result[2][3]);

// ポリゴンリストからAABBを生成
void creatAABBfromTriangles(const std::vector<std::shared_ptr <Triangle>> &triangles,
	float bbox[2][3]);		

// nodeIndex で指定されたノードを、polygons を含む葉ノードにする
void makeLeaf(std::vector<std::shared_ptr <Triangle>> &polygons, BVH_node *node);

// 与えられたポリゴンリストについて、SAH に基づいて領域分割
// nodeIndex は対象ノードのインデックス
void constructBVH_internal(std::vector<std::shared_ptr<Triangle>> &polygons,
	BVH_node *nodes, int used_node_count, int nodeIndex);

// フロントエンド関数．これを呼べば nodes[0] をルートとした BVH が構築される
void constructBVH(std::vector<std::shared_ptr<Triangle>> &polygons,
	BVH_node *nodes, int used_node_count);
