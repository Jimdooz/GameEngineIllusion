#pragma once

#include "../primitives/Primitives.h"		// Get primitives
#include "./ShapeIntersections.h"
#include "./PointTests.h"					// Get point tests

namespace illusion::core::physics::collisions {

	using namespace illusion::core::physics::primitives;

	/**
	 *	TODO
	 */

	struct CollisionManifold {
		bool colliding;
		Vec3 normal;
		float depth;
		util::Array<Vec3> contacts;
	};

	void ResetCollisionManifold(CollisionManifold* result);

	CollisionManifold FindCollisionFeatures(const Sphere& A, const Sphere& B);
	CollisionManifold FindCollisionFeatures(const OBB& A, const Sphere& B);

	//OBB Collisions
	util::Array<Point> GetVertices(const OBB& obb);
	util::Array<Line> GetEdges(const OBB& obb);
	util::Array<Plane> GetPlanes(const OBB& obb);
	bool ClipToPlane(const Plane& plane, const Line& line, Point* outPoint);
	util::Array<Point> ClipEdgesToOBB(const std::vector<Line>& edges, const OBB& obb);
	float PenetrationDepth(const OBB& o1, const OBB& o2, const Vec3& axis, bool* outShouldFlip);
	CollisionManifold FindCollisionFeatures(const OBB& A, const OBB& B);
}