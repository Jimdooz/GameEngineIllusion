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

	float Raycast(const OBB& obb, const Ray& ray) {
		// X, Y and Z axis of OBB
		const Vec3 &size = obb.size;
		Vec3 X = obb.orientation[0];
		Vec3 Y = obb.orientation[1];
		Vec3 Z = obb.orientation[2];
		Vec3 p = obb.position - ray.origin;
		Vec3 f(
			glm::dot(X, ray.direction),
			glm::dot(Y, ray.direction),
			glm::dot(Z, ray.direction)
		);

		Vec3 e(
			glm::dot(X, p),
			glm::dot(Y, p),
			glm::dot(Z, p)
		);
		f32 t[6] = { 0, 0, 0, 0, 0, 0 };
		for (int i = 0; i < 3; ++i) {
			if (CMP(f[i], 0)) {
				if (-e[i] - size[i] > 0 || -e[i] + size[i] < 0) {
					return -1;
				}
				f[i] = 0.00001f; // Avoid div by 0!
			}
			t[i * 2 + 0] = (e[i] + size[i]) / f[i]; // min
			t[i * 2 + 1] = (e[i] - size[i]) / f[i]; // max
		}

		f32 tmin = fmaxf(fmaxf(fminf(t[0], t[1]),fminf(t[2], t[3])),fminf(t[4], t[5]));
		f32 tmax = fminf(fminf(fmaxf(t[0], t[1]),fmaxf(t[2], t[3])),fmaxf(t[4], t[5]));

		if (tmax < 0 || tmin>tmax) return -1;
		if (tmin < 0.0f) return tmax;
		return tmin;
	}

	float Raycast(const Plane& plane, const Ray& ray) {
		f32 nd = glm::dot(ray.direction, plane.normal);
		f32 pn = glm::dot(ray.origin, plane.normal);
		if (nd >= 0.0f) return -1;
		f32 t = (plane.distance - pn) / nd;
		if (t >= 0.0f) return t;
		return -1;
	}

	bool Linetest(const Sphere& sphere, const Line& line) {
		Point closest = ClosestPoint(line, sphere.position);
		float distSq = glm::length2(sphere.position - closest);
		return distSq <= (sphere.radius * sphere.radius);
	}

	bool Linetest(const AABB& aabb, const Line& line) {
		Ray ray;
		ray.origin = line.start;
		ray.direction = glm::normalize(line.end - line.start);
		f32 t = Raycast(aabb, ray);
		return t >= 0 && (t * t) <= Line::LengthSq(line);
	}

	bool Linetest(const OBB& obb, const Line& line) {
		Ray ray;
		ray.origin = line.start;
		ray.direction = glm::normalize(line.end - line.start);
		float t = Raycast(obb, ray);
		return t >= 0 && t * t <= Line::LengthSq(line);
	}

	bool Linetest(const Plane& plane, const Line& line) {
		Vec3 ab = line.end - line.start;
		float nA = glm::dot(plane.normal, line.start);
		float nAB = glm::dot(plane.normal, ab);
		float t = (plane.distance - nA) / nAB;
		return t >= 0.0f && t <= 1.0f;
	}
}