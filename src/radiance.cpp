#include "radiance.h"

// ray��������̕��ˋP�x�����߂�
Color radiance(Scene &scene, Ray &ray, Random &rng, const int maxDepth) {

	int depth = 0;
	Color L = Color(0.0), beta = Color(1.0);
	float pdf = 1.0;

	for (depth = 0;; ++depth) {
		if (depth >= maxDepth) {
			break;
		}

		std::shared_ptr<Triangle> HitTri = nullptr;
		Intersection intersection;
		HitTri = intersect(&scene.nodes[0], 0, ray, &intersection);
		/* �ő��bounce����]��*/

		if (HitTri == nullptr) {
			break;
		}
		const double t = intersection.hitpoint.distance;
		const Triangle &obj = *HitTri;
		const Vec normal = obj.normal;
		const Vec orienting_normal = Dot(normal, ray.dir) < 0.0 ? normal : (-1.0 * normal); // �����ʒu�̖@���i���̂���̃��C�̓��o���l���j
		const Vec hitpoint = ray.org + (t - EPS) * ray.dir;

		//return Color(Dot((-ray.dir), orienting_normal));
		// �Œ�o�E���X��]�����ȏヌ�C��ǐՂ����烍�V�A�����[���b�g�����s���ǐՂ�ł��؂邩�ǂ����𔻒f����
		double russian_roulette_probability = std::max(obj.mat.ref.x, std::max(obj.mat.ref.y, obj.mat.ref.z));
		russian_roulette_probability = std::max(0.05, russian_roulette_probability);
		if (depth > 5) {
			if (rng.next01() > russian_roulette_probability) {
				break;
			}
		}
		else {
			russian_roulette_probability = 1.0;
		}

		// ���C�ƕ��̂̌����_����̕��ˋP�x�`�B���v�Z
			// ���ڌ��̃T���v�����O���s��
		if (!obj.mat.isLight()) {
			if (obj.mat.type == DIFFUSE) {
				Vec direct_light;
				direct_light = direct_radiance_sample(scene, hitpoint, orienting_normal, obj, rng);
				beta = beta * obj.mat.ref / russian_roulette_probability;
				L = L + beta * direct_light;
				Assertion(L.isValid(), "L is invalid: (%f, %f %f)", L.x, L.y, L.z);

				// orienting_normal�̕�������Ƃ������K�������(w, u, v)�����B���̊��ɑ΂��锼�����Ŏ��̃��C���΂��B
				Vec w, u, v;
				w = orienting_normal;
				if (std::abs(w.x) > 0.1)
					u = Normalize(Cross(Vec(0.0, 1.0, 0.0), w));
				else
					u = Normalize(Cross(Vec(1.0, 0.0, 0.0), w));
				v = Cross(w, u);

				// �R�T�C�������g�����d�_�I�T���v�����O
				const double r1 = 2.0 * PI * rng.next01();
				const double r2 = rng.next01();
				const double r2s = sqrt(r2);
				Vec dir = Normalize((u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1.0 - r2)));
				ray = Ray(hitpoint, dir);
				Assertion(beta.isValid(), "beta is invalid: (%f, %f %f)", beta.x, beta.y, beta.z);
			}
			else if (obj.mat.type == SPECULAR) {
				// ���S���ʂȂ̂Ń��C�̔��˕����͌���I�B
				// ���V�A�����[���b�g�̊m���ŏ��Z����̂͏�Ɠ����B
				//Intersection lintersect;//���ˌ��̏��
				beta = beta * obj.mat.ref / russian_roulette_probability;
				Ray reflection_ray = Ray(hitpoint, reflect(ray.dir, normal));
				std::shared_ptr<Triangle> lHitTri = nullptr;
				Intersection lintersection;
				lHitTri = intersect(&scene.nodes[0], 0, reflection_ray, &lintersection);
				if (lHitTri != nullptr) {
					if (lHitTri->mat.isLight()) {
						L = L + beta * direct_radiance(hitpoint, orienting_normal, lHitTri,
							lintersection.hitpoint.position, lintersection);
						break;
					}
					else {
						ray = reflection_ray;
					}
				}
				else {
					break;
				}
			}
			else if (obj.mat.type == REFRACTION) {
				// ���˕�������̒��ڌ�
				beta = beta * obj.mat.ref / russian_roulette_probability;
				Ray reflection_ray(hitpoint - ray.dir * EPS, reflect(ray.dir, normal));
				Intersection lintersection;
				std::shared_ptr<Triangle> lHitTri = nullptr;
				lHitTri = intersect(&scene.nodes[0], 0, reflection_ray, &lintersection);

				bool into = Dot(normal, orienting_normal) > 0.0; // ���C���I�u�W�F�N�g����o��̂��A����̂��A�o��Ȃ��false
																 // Snell�̖@��
				const double nc = 1.0; // �^��̋��ܗ�
				const double nt = 1.5; // �I�u�W�F�N�g�̋��ܗ�
				const double nnt = into ? nc / nt : nt / nc;
				const double ddn = Dot(ray.dir, orienting_normal);
				const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
				if (cos2t < 0.0) { // �S���˂���
					if (lHitTri != nullptr) {
						if (lHitTri->mat.isLight()) {
							L = L + beta * direct_radiance(hitpoint, orienting_normal, lHitTri,
								lintersection.hitpoint.position, lintersection);
							break;
						}
						else {
							ray = reflection_ray;
							continue;
						}
					}
					break;
				}
				// ���܂��Ă�������
				Vec tdir = Normalize(ray.dir * nnt - normal * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t)));
				// Schlick�ɂ��Fresnel�̔��ˌW���̋ߎ�
				const double a = nt - nc, b = nt + nc;
				const double R0 = (a * a) / (b * b);
				const double c = 1.0 - (into ? -ddn : Dot(tdir, normal));
				const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
				const double Tr = 1.0 - Re; // ���܌��̉^�Ԍ��̗�
				const double probability = 0.25 + 0.5 * Re;
				// ���ܕ�������̒��ڌ��T���v�����O����
				Ray refraction_ray = Ray(hitpoint + 2.0 * EPS * ray.dir, tdir);
				Intersection r_lintersection;
				std::shared_ptr<Triangle> r_lHitTri;
				r_lHitTri = nullptr;
				r_lHitTri = intersect(&scene.nodes[0], 0, refraction_ray, &r_lintersection);

				// ���ȏヌ�C��ǐՂ�������܂Ɣ��˂̂ǂ��炩�����ǐՂ���B�i�����Ȃ��Ǝw���I�Ƀ��C��������j
				// ���V�A�����[���b�g�Ō��肷��B
				if (false/*rng.next01() < probability*/) {// ����
					beta = beta / probability * Re;
					if (lHitTri != nullptr) {
						if (lHitTri->mat.isLight()) {
							L = L + beta * direct_radiance(hitpoint, orienting_normal, lHitTri,
								lintersection.hitpoint.position, lintersection);
							break;
						}
						else {
							ray = reflection_ray;
						}
					}
					else {
						break;
					}
				}
				else { // ����
					beta = beta / (1.0 - probability) * (1.0 - Re);
					if (r_lHitTri != nullptr) {
						if (r_lHitTri->mat.isLight()) {
							L = L + beta * direct_radiance(hitpoint, orienting_normal, r_lHitTri,
								r_lintersection.hitpoint.position, r_lintersection);
							break;
						}
						else {
							ray = refraction_ray;
						}
					}
					else {
						break;
					}
				}
			}
		}
		else if (depth == 0) {
			if (Dot(-ray.dir, obj.normal) > 0.0) {
				L = obj.mat.Le;
				break;
			}
			else {
				L = Color(0.0);
				break;
			}
		}
		else {
			break;
		}
	}
	return L;
}

// ������̓_���T���v�����O���Ē��ڌ����v�Z����B//DIFFUSE�ʂŗp����
Color direct_radiance_sample(Scene &scene, const Vec &v0,
	const Vec &normal, const Triangle& objtri, Random &rng) {

	// 1�̌������b�V�����T���v�����O����
	const int r1 = scene.Ltris.size() * rng.next01();
	const auto ltri = scene.Ltris[r1];
	const TriangleMesh &lmesh = ltri->mesh;

	//���b�V����̈�_���T���v��
	const double u1 = rng.next01();
	Vec lpos = lmesh.p[0] + u1 * (lmesh.p[1] - lmesh.p[0]) + (1.0 - u1) * (lmesh.p[2] - lmesh.p[0]);

	//ray�𐶐���hit�������b�V�����T���v���������̂ƈ�v����Β��ڌ��̌v�Z
	Vec dir = Normalize(lpos - v0);
	Intersection lintersection;
	std::shared_ptr<Triangle> LHitTri = nullptr;
	LHitTri = intersect(&scene.Lnodes[0], 0, Ray(v0, dir), &lintersection);

	if (LHitTri == ltri) {
		return direct_radiance(v0, normal, ltri, lpos, lintersection)
			* (float)scene.Ltris.size();
	}
	return Color(0.0);
}

Color direct_radiance(const Vec &v0, const Vec &normal, const std::shared_ptr<Triangle> ltri,
	const Vec &lpos, const Intersection &lintersect) {
	//tri �͌����ł͂Ȃ����Ƃ�obj�̃��b�V��
	const Vec lnormal = ltri->normal;
	const Vec ldir = Normalize(lpos - v0);
	const double dist2 = (lpos - v0).LengthSquared();
	const double dot0 = Dot(normal, ldir);
	const double dot1 = Dot(lnormal, -1.0 * ldir);

	if (dot0 >= 0.0 && dot1 >= 0.0) {
		const double G = dot0 * dot1 / dist2;
		if (std::abs(sqrt(dist2) - lintersect.hitpoint.distance) < 1e-3) {
			Vec edge1 = (ltri->mesh.p[1] - ltri->mesh.p[0]);
			Vec edge2 = (ltri->mesh.p[2] - ltri->mesh.p[0]);
			double S = Cross(edge1, edge2).Length() / 2.0;//light�̃��b�V����̕\�ʐ�
			return ltri->mat.Le * G * S;
		}
	}
	return Color(0.0);
}