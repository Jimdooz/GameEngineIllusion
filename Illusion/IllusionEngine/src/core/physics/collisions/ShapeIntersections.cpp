#include "core/physics/collisions/ShapeIntersections.h"

using namespace illusion::core::physics::primitives;

namespace illusion::core::physics::collisions {

	bool SphereSphere(const Sphere& s1, const Sphere& s2) {
		f32 radiiSum = s1.radius + s2.radius;
		f32 sqDistance = glm::length2(s1.position - s2.position);
		return sqDistance < radiiSum* radiiSum;
	}

	bool SphereAABB(const Sphere& sphere, const AABB& aabb) {
		Point closestPoint = ClosestPoint(aabb, sphere.position);
		f32 distSq = glm::length2(sphere.position - closestPoint);
		f32 radiusSq = sphere.radius * sphere.radius;
		return distSq < radiusSq;
	}

	bool SphereOBB(const Sphere& sphere, const OBB& obb) {
		Point closestPoint = ClosestPoint(obb, sphere.position);
		f32 distSq = glm::length2(sphere.position - closestPoint);
		f32 radiusSq = sphere.radius * sphere.radius;
		return distSq < radiusSq;
	}

	bool SpherePlane(const Sphere& sphere, const Plane& plane) {
		Point closestPoint = ClosestPoint(plane, sphere.position);
		float distSq = glm::length2(sphere.position - closestPoint);
		float radiusSq = sphere.radius * sphere.radius;
		return distSq < radiusSq;
	}

	bool AABBAABB(const AABB& aabb1, const AABB& aabb2) {
		Point aMin = AABB::GetMin(aabb1);
		Point aMax = AABB::GetMax(aabb1);
		Point bMin = AABB::GetMin(aabb2);
		Point bMax = AABB::GetMax(aabb2);

		return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
			(aMin.y <= bMax.y && aMax.y >= bMin.y) &&
			(aMin.z <= bMax.z && aMax.z >= bMin.z);
	}
}