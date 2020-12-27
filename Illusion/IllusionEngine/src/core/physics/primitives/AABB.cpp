#include "core/physics/primitives/AABB.h"

namespace illusion::core::physics::primitives {

	Vec3 AABB::GetMin(const AABB& aabb){
		Vec3 p1 = aabb.position + aabb.size;
		Vec3 p2 = aabb.position - aabb.size;
		return Vec3(fminf(p1.x, p2.x),
					fminf(p1.y, p2.y),
					fminf(p1.z, p2.z));
	}

	Vec3 AABB::GetMax(const AABB& aabb){
		Vec3 p1 = aabb.position + aabb.size;
		Vec3 p2 = aabb.position - aabb.size;
		return Vec3(fmaxf(p1.x, p2.x),
					fmaxf(p1.y, p2.y),
					fmaxf(p1.z, p2.z));
	}

	AABB AABB::FromMinMax(const Vec3& min, const Vec3& max){
		return AABB((min + max) * 0.5f, (max - min) * 0.5f);
	}

}