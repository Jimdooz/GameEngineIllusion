#pragma once

#include "../primitives/Primitives.h"


namespace illusion::core::physics::collisions {

	using namespace illusion::core::physics::primitives;

	//Sphere
	bool PointInSphere(const Point& point, const Sphere& sphere);
	Point ClosestPoint(const Sphere& sphere, const Point& point);

	//AABB
	bool PointInAABB(const Point& point, const AABB& aabb);
	Point ClosestPoint(const AABB& aabb, const Point& point);

	//OBB
	bool PointInOBB(const Point& point, const OBB& obb);
	Point ClosestPoint(const OBB& obb, const Point& point);

	//Plane
	bool PointOnPlane(const Point& point, const Plane& plane);
	Point ClosestPoint(const Plane& plane, const Point& point);

	//Line
	bool PointOnLine(const Point& point, const Line& line);
	Point ClosestPoint(const Line& line, const Point& point);

	//Ray
	bool PointOnRay(const Point& point, const Ray& ray);
	Point ClosestPoint(const Ray& ray, const Point& point);
}