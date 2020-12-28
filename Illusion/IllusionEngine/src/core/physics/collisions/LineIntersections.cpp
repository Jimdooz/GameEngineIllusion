#include "core/physics/collisions/LineIntersections.h"

using namespace illusion::core::physics::primitives;

namespace illusion::core::physics::collisions {

	float Raycast(const Sphere& sphere, const Ray& ray) {
		Vec3 e = sphere.position - ray.origin;
		f32 rSq = sphere.radius * sphere.radius;
		f32 eSq = glm::length2(e);

		// ray.direction is assumed to be normalized
		f32 a = glm::dot(e, ray.direction);

		f32 bSq = eSq - (a * a);
		f32 f = sqrt(rSq - bSq);

		// No collision has happened
		if (rSq - (eSq - (a * a)) < 0.0f) return -1; // -1 is invalid.
		// Ray starts inside the sphere
		else if (eSq < rSq) return a + f; // Just reverse direction
		// else Normal intersection
		return a - f;
	}

	float Raycast(const AABB& aabb, const Ray& ray) {
		Vec3 min = AABB::GetMin(aabb);
		Vec3 max = AABB::GetMax(aabb);

		f32 dirX = ray.direction.x == 0 ? EPSILON : ray.direction.x;
		f32 dirY = ray.direction.y == 0 ? EPSILON : ray.direction.y;
		f32 dirZ = ray.direction.z == 0 ? EPSILON : ray.direction.z;

		f32 t1 = (min.x - ray.origin.x) / dirX;
		f32 t2 = (max.x - ray.origin.x) / dirX;
		f32 t3 = (min.y - ray.origin.y) / dirY;
		f32 t4 = (max.y - ray.origin.y) / dirY;
		f32 t5 = (min.z - ray.origin.z) / dirZ;
		f32 t6 = (max.z - ray.origin.z) / dirZ;

		f32 tmin = fmaxf(fmaxf(fminf(t1, t2),fminf(t3, t4)),fminf(t5, t6));
		f32 tmax = fminf(fminf(fmaxf(t1, t2),fmaxf(t3, t4)),fmaxf(t5, t6));

		if (tmax < 0 || tmin>tmax) return -1;
		if (tmin < 0.0f) return tmax;
		return tmin;
	}

}