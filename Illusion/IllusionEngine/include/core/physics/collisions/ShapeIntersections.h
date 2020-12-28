#pragma once

#include "../primitives/Primitives.h"		// Get primitives
#include "./PointTests.h"					// Get point tests

using namespace illusion::core::physics::primitives;

namespace illusion::core::physics::collisions {

	/**
	 *		Tests dones : 
	 *
	 *					Sphere	AABB	OBB		Plane
	 *		Sphere		x		x		x		x
	 *		AABB		x		x		x		x
	 *		OBB			x		x		x		x
	 *		Plane		x		x		x		x
	 * 
	 */

	// For OBB Tests collisions
	struct Interval {
		f32 min;
		f32 max;
	};

	bool SphereSphere(const Sphere& s1, const Sphere& s2);
	bool SphereAABB(const Sphere& sphere, const AABB& aabb);
	bool SphereOBB(const Sphere& sphere, const OBB& obb);
	bool SpherePlane(const Sphere& sphere, const Plane& plane);

	bool AABBAABB(const AABB& aabb1, const AABB& aabb2);

	Interval GetInterval(const AABB& aabb, const Vec3& axis);
	Interval GetInterval(const OBB& obb, const Vec3& axis);
	bool OverlapOnAxis(const AABB& aabb, const OBB& obb, const Vec3& axis);
	bool OverlapOnAxis(const OBB& obb1, const OBB& obb2, const Vec3& axis);

	bool AABBOBB(const AABB& aabb, const OBB& obb);
	bool AABBPlane(const AABB& aabb, const Plane& plane);

	bool OBBOBB(const OBB& obb1, const OBB& obb2);
	bool OBBPlane(const OBB& obb, const Plane& plane);

	bool PlanePlane(const Plane& plane1, const Plane& plane2);

	// Mirror Call
	constexpr bool AABBSphere(const AABB& aabb, const Sphere& sphere) { return SphereAABB(sphere, aabb); }
	constexpr bool OBBSphere(const OBB& obb, const Sphere& sphere) { return SphereOBB(sphere, obb); }
	constexpr bool PlaneSphere(const Plane& plane, const Sphere& sphere) { return SpherePlane(sphere, plane); }
	constexpr bool OBBAABB(const OBB& obb, const AABB& aabb){ return AABBOBB(aabb, obb); }
	constexpr bool PlaneAABB(const Plane& plane, const AABB& aabb) { return AABBPlane(aabb, plane); }
	constexpr bool PlaneOBB(const Plane& plane, const OBB& obb) { return OBBPlane(obb, plane); }
}