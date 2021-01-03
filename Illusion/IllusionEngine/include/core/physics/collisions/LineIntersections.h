#pragma once

#include "../primitives/Primitives.h"		// Get primitives
#include "./PointTests.h"					// Get point tests


namespace illusion::core::physics::collisions {

	using namespace illusion::core::physics::primitives;

	/**
	 *		Tests dones :
	 *
	 *					Sphere	AABB	OBB		Plane
	 *		Raycast		x		x		x		x
	 *		Linetest	x		x		x		x
	 *
	 */

	struct RaycastResult {
		Vec3 point;
		Vec3 normal;
		f32 t;
		bool hit;
	};

	void ResetRaycastResult(RaycastResult* outResult);

	/**
	 * Raycast intersections will return the distance along the ray that the intersection has happened.
	 * Line segment intersections will simply return a Boolean value
	 */
	f32 Raycast(const Sphere& sphere, const Ray& ray);
	f32 Raycast(const AABB& aabb, const Ray& ray);
	f32 Raycast(const OBB& obb, const Ray& ray);
	f32 Raycast(const Plane& plane, const Ray& ray);

	bool Linetest(const Sphere& sphere, const Line& line);
	bool Linetest(const AABB& aabb, const Line& line);
	bool Linetest(const OBB& obb, const Line& line);
	bool Linetest(const Plane& plane, const Line& line);

	//Raycast with informations
	bool Raycast(const Sphere& sphere, const Ray& ray, RaycastResult* outResult);
	bool Raycast(const AABB& aabb, const Ray& ray, RaycastResult* outResult);
	bool Raycast(const OBB& obb, const Ray& ray, RaycastResult* outResult);
	bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult);

	/**
	 * Line Intersections
	 */
	bool LineLineIntersect(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, Vec3* pa, Vec3* pb, double* mua, double* mub);
	bool LineLineIntersect(const Line& A, const Line& B, Line* Result);
	bool RayRayIntersect(const Ray& A, const Ray& B, Line* Result);
}