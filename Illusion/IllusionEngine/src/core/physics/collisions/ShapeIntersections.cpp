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

	Interval GetInterval(const AABB& aabb, const Vec3& axis) {
		Vec3 i = AABB::GetMin(aabb);
		Vec3 a = AABB::GetMax(aabb);
		Vec3 vertex[8] = {
			Vec3(i.x, a.y, a.z),
			Vec3(i.x, a.y, i.z),
			Vec3(i.x, i.y, a.z),
			Vec3(i.x, i.y, i.z),
			Vec3(a.x, a.y, a.z),
			Vec3(a.x, a.y, i.z),
			Vec3(a.x, i.y, a.z),
			Vec3(a.x, i.y, i.z)
		};

		Interval result;
		result.min = result.max = glm::dot(axis, vertex[0]);
		for (int i = 1; i < 8; ++i) {
			float projection = glm::dot(axis, vertex[i]);
			result.min = (projection < result.min) ? projection : result.min;
			result.max = (projection > result.max) ? projection : result.max;
		}
		return result;
	}

	Interval GetInterval(const OBB& obb, const Vec3& axis) {
		Vec3 vertex[8];
		Vec3 C = obb.position;
		Vec3 E = obb.size;
		const Mat3x3& A = obb.orientation;
		vertex[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
		vertex[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
		vertex[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
		vertex[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
		vertex[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
		vertex[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
		vertex[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
		vertex[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

		Interval result;
		result.min = result.max = glm::dot(axis, vertex[0]);
		for (int i = 1; i < 8; ++i) {
			float projection = glm::dot(axis, vertex[i]);
			result.min = (projection < result.min) ? projection : result.min;
			result.max = (projection > result.max) ? projection : result.max;
		}
		return result;
	}

	bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const Vec3& axis) {
		Interval a = GetInterval(aabb, axis);
		Interval b = GetInterval(obb, axis);
		return ((b.min <= a.max) && (a.min <= b.max));
	}

	bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const Vec3& axis) {
		Interval a = GetInterval(obb1, axis);
		Interval b = GetInterval(obb1, axis);
		return ((b.min <= a.max) && (a.min <= b.max));
	}

	bool AABBOBB(const AABB& aabb, const OBB& obb) {
		const Mat3x3& o = obb.orientation;
		Vec3 test[15] = {
			Vec3(1, 0, 0), // AABB axis 1
			Vec3(0, 1, 0), // AABB axis 2
			Vec3(0, 0, 1), // AABB axis 3
			o[0],	// OBB axis 1
			o[1],	// OBB axis 2
			o[2]	// OBB axis 3
		};

		for (int i = 0; i < 3; ++i) { // Fill out rest of axis
			test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
			test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
			test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
		}

		for (int i = 0; i < 15; ++i) {
			if (!OverlapOnAxis(aabb, obb, test[i])) return false;
		}
		return true; // Seperating axis not found
	}

	bool AABBPlane(const AABB& aabb, const Plane& plane) {
		f32 pLen = aabb.size.x * fabsf(plane.normal.x) +
			aabb.size.y * fabsf(plane.normal.y) +
			aabb.size.z * fabsf(plane.normal.z);
		f32 dot = glm::dot(plane.normal, aabb.position);
		f32 dist = dot - plane.distance;
		return fabsf(dist) <= pLen;
	}

	bool OBBOBB(const OBB& obb1, const OBB& obb2) {
		Vec3 test[15] = {
			obb1.orientation[0], obb1.orientation[1], obb1.orientation[2],
			obb2.orientation[0], obb2.orientation[1], obb2.orientation[2]
		};

		for (int i = 0; i < 3; ++i) { // Fill out rest of axis
			test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
			test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
			test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
		}

		for (int i = 0; i < 15; ++i) {
			if (!OverlapOnAxis(obb1, obb2, test[i])) return false; // Seperating axis found
		}

		return true; // Seperating axis not found
	}

	bool OBBPlane(const OBB& obb, const Plane& plane) {
		Vec3 rot[] = { // rotation / orientation
			obb.orientation[0],
			obb.orientation[1],
			obb.orientation[2],
		};
		Vec3 normal = plane.normal;

		f32 pLen = obb.size.x * fabsf(glm::dot(normal, rot[0])) +
			obb.size.y * fabsf(glm::dot(normal, rot[1])) +
			obb.size.z * fabsf(glm::dot(normal, rot[2]));

		float dot = glm::dot(plane.normal, obb.position);
		float dist = dot - plane.distance;

		return fabsf(dist) <= pLen;
	}

	bool PlanePlane(const Plane& plane1, const Plane& plane2) {
		// Cross product returns 0 when used on parallel lines
		Vec3 d = glm::cross(plane1.normal, plane2.normal);
		return !NEAR_EPSILON(glm::dot(d, d)); // Consider using an epsilon!
	}
}