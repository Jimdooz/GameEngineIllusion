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
		float distSq = glm::length2(sphere.position - closestPoint);
		float radiusSq = sphere.radius * sphere.radius;
		return distSq < radiusSq;
	}

}