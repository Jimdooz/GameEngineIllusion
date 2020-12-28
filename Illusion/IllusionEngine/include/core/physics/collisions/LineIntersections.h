#pragma once

#include "../primitives/Primitives.h"		// Get primitives
#include "./PointTests.h"					// Get point tests

using namespace illusion::core::physics::primitives;

namespace illusion::core::physics::collisions {

	/**
	 *		Tests dones :
	 *
	 *					Sphere	AABB	OBB		Plane
	 *		Raycast		x		x		x		x
	 *		Linetest	-		-		-		-
	 *
	 */

	/**
	 * Raycast intersections will return the distance along the ray that the intersection has happened.
	 * Line segment intersections will simply return a Boolean value
	 */
	float Raycast(const Sphere& sphere, const Ray& ray);
	float Raycast(const AABB& aabb, const Ray& ray);
	float Raycast(const OBB& obb, const Ray& ray);
	float Raycast(const Plane& plane, const Ray& ray);

	// Mirror Call

}