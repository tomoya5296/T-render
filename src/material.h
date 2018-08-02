#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include "vec.h"

enum RefType : int {
	DIFFUSE,
	SPECULAR,
	REFRACTION,
	TRANSLUCENT,
};

class Material {
public:
	Material(const Color &Le_, const Color &ref_, RefType type_)
		: Le(Le_)
		, ref(ref_)
		, type(type_) {}

	Material(){
		Le = Color(0.0);
		ref = Color (1.0);
		type = DIFFUSE;
	}

	bool isLight() const {
		return !Le.isZero();
	}

	Color Le, ref;
	RefType type;
};

#endif  // _MATERIAL_H_
