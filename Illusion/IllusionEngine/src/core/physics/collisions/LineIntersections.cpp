#include "core/physics/collisions/LineIntersections.h"

using namespace illusion::core::physics::primitives;

namespace illusion::core::physics::collisions {

	void ResetRaycastResult(RaycastResult* outResult) {
		if (outResult != 0) {
			outResult->t = -1;
			outResult->hit = false;
			outResult->normal = Vec3(0, 0, 1);
			outResult->point = Vec3(0, 0, 0);
		}
	}

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
				f[i] = 0.000001f; // Avoid div by 0!
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
		RaycastResult raycast;
		if (!Raycast(aabb, ray, &raycast)) return false;
		f32 t = raycast.t;
		return t >= 0 && (t * t) <= Line::LengthSq(line);
	}

	bool Linetest(const OBB& obb, const Line& line) {
		Ray ray;
		ray.origin = line.start;
		ray.direction = glm::normalize(line.end - line.start);

		RaycastResult result;
		if (!Raycast(obb, ray, &result)) return false;
		float t = result.t;
		return t >= 0 && (t * t) <= Line::LengthSq(line);
	}

	bool Linetest(const Plane& plane, const Line& line) {
		Vec3 ab = line.end - line.start;
		float nA = glm::dot(plane.normal, line.start);
		float nAB = glm::dot(plane.normal, ab);
		float t = (plane.distance - nA) / nAB;
		return t >= 0.0f && t <= 1.0f;
	}

	bool Raycast(const Sphere& sphere, const Ray& ray, RaycastResult* outResult) {
		ResetRaycastResult(outResult);
		Vec3 e = sphere.position - ray.origin;
		f32 rSq = sphere.radius * sphere.radius;
		f32 eSq = glm::length2(e);
		f32 a = glm::dot(e, ray.direction);
		f32 bSq = eSq - (a * a);
		f32 f = sqrt(rSq - bSq);
		f32 t = a - f;

		if (rSq - (eSq - a * a) < 0.0f) return false;
		else if (eSq < rSq) { // Inside sphere
			t = a + f; // Reverse direction
		}

		if (outResult != 0) {
			outResult->t = t;
			outResult->hit = true;
			outResult->point = ray.origin + ray.direction * t;
			outResult->normal = glm::normalize(outResult->point - sphere.position);
		}
		return true;
	}

	bool Raycast(const AABB& aabb, const Ray& ray, RaycastResult* outResult) {
		ResetRaycastResult(outResult);
		Vec3 min = AABB::GetMin(aabb);
		Vec3 max = AABB::GetMax(aabb);

		f32 dirX = ray.direction.x == 0 ? EPSILON : ray.direction.x;
		f32 dirY = ray.direction.y == 0 ? EPSILON : ray.direction.y;
		f32 dirZ = ray.direction.z == 0 ? EPSILON : ray.direction.z;

		f32 t[] = { 0, 0, 0, 0, 0, 0 };
		t[0] = (min.x - ray.origin.x) / dirX;
		t[1] = (max.x - ray.origin.x) / dirX;
		t[2] = (min.y - ray.origin.y) / dirY;
		t[3] = (max.y - ray.origin.y) / dirY;
		t[4] = (min.z - ray.origin.z) / dirZ;
		t[5] = (max.z - ray.origin.z) / dirZ;

		f32 tmin = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
		f32 tmax = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

		if (tmax < 0 || tmin>tmax) return false;
		f32 t_result = tmin;
		if (tmin < 0.0f) { t_result = tmax; }

		if (outResult != 0) {
			outResult->t = t_result;
			outResult->hit = true;
			outResult->point = ray.origin +
				ray.direction * t_result;
			Vec3 normals[] = {
				Vec3(-1, 0, 0), Vec3(1, 0, 0),
				Vec3(0, -1, 0), Vec3(0, 1, 0),
				Vec3(0, 0, -1), Vec3(0, 0, 1)
			};
			for (int i = 0; i < 6; i++) {
				if (CMP(t_result, t[i])) {
					outResult->normal = normals[i];
				}
			}
		}

		return true;
	}

	bool Raycast(const OBB& obb, const Ray& ray, RaycastResult* outResult) {
		ResetRaycastResult(outResult);
		const Vec3& size = obb.size;
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

		f32 tmin = fmaxf(fmaxf(fminf(t[0], t[1]), fminf(t[2], t[3])), fminf(t[4], t[5]));
		f32 tmax = fminf(fminf(fmaxf(t[0], t[1]), fmaxf(t[2], t[3])), fmaxf(t[4], t[5]));

		if (tmax < 0 || tmin>tmax) return false;
		
		float t_result = tmin;
		if (tmin < 0.0f) { t_result = tmax; }

		if (outResult != 0) {
			outResult->hit = true;
			outResult->t = t_result;
			outResult->point = ray.origin + ray.direction * t_result;
			Vec3 normals[] = { X, X * -1.0f, Y, Y * -1.0f, Z, Z * -1.0f };
			for (int i = 0; i < 6; ++i) {
				if (CMP(t_result, t[i])) {
					outResult->normal = glm::normalize(normals[i]);
				}
			}
		}

		return true;
	}

	bool Raycast(const Plane& plane, const Ray& ray, RaycastResult* outResult) {
		ResetRaycastResult(outResult);
		f32 nd = glm::dot(ray.direction, plane.normal);
		f32 pn = glm::dot(ray.origin, plane.normal);
		if (nd >= 0.0f) return false;
		f32 t = (plane.distance - pn) / nd;
		if (t >= 0.0f) {
			if (outResult != 0) {
				outResult->t = t;
				outResult->hit = true;
				outResult->point = ray.origin + ray.direction * t;
				outResult->normal = glm::normalize(plane.normal);
			}
			return true;
		}
		return false;
	}

	bool LineLineIntersect(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, Vec3* pa, Vec3* pb, double* mua, double* mub) {
		Vec3 p13, p43, p21;
		f32 d1343, d4321, d1321, d4343, d2121;
		f32 numer, denom;

		p13.x = p1.x - p3.x;
		p13.y = p1.y - p3.y;
		p13.z = p1.z - p3.z;
		p43.x = p4.x - p3.x;
		p43.y = p4.y - p3.y;
		p43.z = p4.z - p3.z;
		if (abs(p43.x) < EPSILON && abs(p43.y) < EPSILON && abs(p43.z) < EPSILON) return false;
		p21.x = p2.x - p1.x;
		p21.y = p2.y - p1.y;
		p21.z = p2.z - p1.z;
		if (abs(p21.x) < EPSILON && abs(p21.y) < EPSILON && abs(p21.z) < EPSILON) return false;

		d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
		d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
		d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
		d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
		d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

		denom = d2121 * d4343 - d4321 * d4321;
		if (abs(denom) < EPSILON) return false;
		numer = d1343 * d4321 - d1321 * d4343;

		*mua = numer / denom;
		*mub = (d1343 + d4321 * (*mua)) / d4343;

		pa->x = p1.x + *mua * p21.x;
		pa->y = p1.y + *mua * p21.y;
		pa->z = p1.z + *mua * p21.z;
		pb->x = p3.x + *mub * p43.x;
		pb->y = p3.y + *mub * p43.y;
		pb->z = p3.z + *mub * p43.z;

		return true;
	}

	bool LineLineIntersect(const Line& A, const Line& B, Line* Result) {
		Vec3 p1, p2;
		f64 mua, mub;

		bool result = LineLineIntersect(A.start, A.end, B.start, B.end, &p1, &p2, &mua, &mub);
		if (!result) return false;

		Result->start = p1;
		Result->end = p2;
		return true;
	}

	bool RayRayIntersect(const Ray& A, const Ray& B, Line* Result) {
		return LineLineIntersect(Line(A.origin, A.origin + A.direction * 1000000000), Line(B.origin, B.origin + B.direction * 1000000000), Result);
	}
}