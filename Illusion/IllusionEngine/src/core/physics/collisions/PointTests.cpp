#include "core/physics/collisions/PointTests.h"

using namespace illusion::core::physics::primitives;

namespace illusion::core::physics::collisions {

	//Sphere
	bool PointInSphere(const Point& point, const Sphere& sphere) {
		f32 magSq = glm::length2(point - sphere.position);
		f32 radSq = sphere.radius * sphere.radius;
		return magSq < radSq;
	}

	Point ClosestPoint(const Sphere& sphere, const Point& point) {
		Vec3 sphereToPoint = point - sphere.position;
		glm::normalize(sphereToPoint);
		sphereToPoint = sphereToPoint * sphere.radius;
		return sphereToPoint + sphere.position;
	}


	//AABB
	bool PointInAABB(const Point& point, const AABB& aabb){
		Point min = AABB::GetMin(aabb);
		Point max = AABB::GetMax(aabb);

		if (point.x > max.x || point.y > max.y || point.z > max.z) {
			return false;
		}
		return true;
	}

	Point ClosestPoint(const AABB& aabb, const Point& point) {
		Point result = point;
		Point min = AABB::GetMin(aabb);
		Point max = AABB::GetMax(aabb);

		result.x = (result.x < min.x) ? min.x : result.x;
		result.y = (result.y < min.x) ? min.y : result.y;
		result.z = (result.z < min.x) ? min.z : result.z;

		result.x = (result.x > max.x) ? max.x : result.x;
		result.y = (result.y > max.x) ? max.y : result.y;
		result.z = (result.z > max.x) ? max.z : result.z;
		return result;
	}

	//OBB
	bool PointInOBB(const Point& point, const OBB& obb) {
		Vec3 dir = point - obb.position;

		for (i32 i = 0; i < 3; i++) {
			Vec3 axis = obb.orientation[i];
			f64 distance = glm::dot(dir, axis);


			if (distance > obb.size[i] + EPSILON) {
				return false;
			}
			if (distance < -obb.size[i] - EPSILON) {
				return false;
			}
		}
		return true;
	}

	Point ClosestPoint(const OBB& obb, const Point& point) {
		Point result = obb.position;
		Vec3 dir = point - obb.position;
		for (i32 i = 0; i < 3; i++) {
			Vec3 axis = obb.orientation[i];
			f32 distance = glm::dot(dir, axis);
			if (distance > obb.size[i]) {
				distance = obb.size[i];
			}
			if (distance < -obb.size[i]) {
				distance = -obb.size[i];
			}
			result += (axis * distance);
		}
		return result;
	}

	//Plane
	bool PointOnPlane(const Point& point, const Plane& plane) {
		float dot = glm::dot(point, plane.normal);
		return CMP(dot - plane.distance, 0.0f);
	}

	Point ClosestPoint(const Plane& plane, const Point& point) {
		float dot = glm::dot(plane.normal, point);
		float distance = dot - plane.distance;
		return point - (plane.normal * distance);
	}

	//Line
	bool PointOnLine(const Point& point, const Line& line) {
		Point closest = ClosestPoint(line, point);
		float distanceSq = glm::length2(closest - point);
		return CMP(distanceSq, 0.0f);
	}

	Point ClosestPoint(const Line& line, const Point& point) {
		Vec3 lVec = line.end - line.start; // Line Vector
		float t = glm::dot(point - line.start, lVec) / glm::dot(lVec, lVec);
		t = fmaxf(t, 0.0f); // Clamp to 0
		t = fminf(t, 1.0f); // Clamp to 1
		return line.start + lVec * t;
	}

	//Ray
	bool PointOnRay(const Point& point, const Ray& ray) {
		if (point == ray.origin) return true;
		Vec3 norm = point - ray.origin;
		glm::normalize(norm);
		float diff = glm::dot(norm, ray.direction);
		return NEAR_EPSILON(diff - 1.0f);
	}

	Point ClosestPoint(const Ray& ray, const Point& point) {
		float t = glm::dot(point - ray.origin, ray.direction);
		t = fmaxf(t, 0.0f);
		return Point(ray.origin + ray.direction * t);
	}

}