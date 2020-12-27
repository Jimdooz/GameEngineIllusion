#pragma once

#include "../primitives/Primitives.h"

using namespace illusion::core::physics::primitives;

namespace illusion::core::physics::collisions {

	//Sphere
	bool PointInSphere(const Point& point, const Sphere& sphere);
	Point ClosestPoint(const Sphere& sphere, const Point& point);

	//AABB
	bool PointInAABB(const Point& point, const AABB& aabb);
	Point ClosestPoint(const AABB& aabb, const Point& point);

	//OBB
	bool PointInOBB(const Point& point, const OBB& obb);
	Point ClosestPoint(const OBB& obb, const Point& point);
}