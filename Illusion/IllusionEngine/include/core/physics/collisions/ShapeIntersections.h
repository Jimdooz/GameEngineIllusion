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
	 *		AABB		x		x		-		-
	 *		OBB			x		-		-		-
	 *		Plane		x		-		-		-
	 * 
	 */

	bool SphereSphere(const Sphere& s1, const Sphere& s2);
	bool SphereAABB(const Sphere& sphere, const AABB& aabb);
	bool SphereOBB(const Sphere& sphere, const OBB& obb);
	bool SpherePlane(const Sphere& sphere, const Plane& plane);

	bool AABBAABB(const AABB& aabb1, const AABB& aabb2);

	// Mirror Call
	constexpr bool AABBSphere(const AABB& aabb, const Sphere& sphere) { return SphereAABB(sphere, aabb); }
	constexpr bool SphereOBB(const OBB& obb, const Sphere& sphere) { return SphereOBB(obb, sphere); }
	constexpr bool SpherePlane(const Plane& plane, const Sphere& sphere) { return SpherePlane(sphere, plane); }
}