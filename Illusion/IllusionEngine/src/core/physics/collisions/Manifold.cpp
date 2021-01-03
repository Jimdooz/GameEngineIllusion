#include "core/physics/collisions/Manifold.h"

using namespace illusion::core::physics::primitives;

namespace illusion::core::physics::collisions {
	
	void ResetCollisionManifold(CollisionManifold* result) {
		if (result != 0) {
			result->colliding = false;
			result->normal = Vec3(0, 0, 1);
			result->depth = FLT_MAX;
			result->contacts.clear();
		}
	}

	CollisionManifold FindCollisionFeatures(const Sphere& A, const Sphere& B) {
		CollisionManifold result;
		ResetCollisionManifold(&result);

		f32 r = A.radius + B.radius;
		Vec3 d = B.position - A.position;

		if (glm::length2(d) - r * r > 0 || glm::length2(d) == 0.0f) return result; // No intersections

		d = glm::normalize(d);

		result.colliding = true;
		result.normal = d;
		result.depth = fabsf(glm::length(d) - r) * 0.5f;
		float dtp = A.radius - result.depth;
		Point contact = A.position + d * dtp;
		result.contacts.push_back(contact);
		return result;
	}

	CollisionManifold FindCollisionFeatures(const OBB& A, const Sphere& B) {
		CollisionManifold result;
		ResetCollisionManifold(&result);

		Point closestPoint = ClosestPoint(A, B.position);
		float distanceSq = glm::length2(closestPoint - B.position);
		if (distanceSq > B.radius * B.radius) return result;

		Vec3 normal;
		if (CMP(distanceSq, 0.0f)) {
			float mSq = glm::length2(closestPoint - A.position);
			if (CMP(mSq, 0.0f)) {
				return result;
			}
			normal = glm::normalize(closestPoint - A.position);
		}
		else {
			normal = glm::normalize(B.position - closestPoint);
		}

		Point outsidePoint = B.position - normal * B.radius;
		float distance = glm::length(closestPoint - outsidePoint);
		result.colliding = true;
		result.contacts.push_back(closestPoint + (outsidePoint - closestPoint) * 0.5f);
		result.normal = normal;
		result.depth = distance * 0.5f;
		return result;
	}

	util::Array<Point> GetVertices(const OBB& obb) {
		util::Array<Vec3> v;
		v.resize(8);
		Vec3 C = obb.position; // OBB Center
		Vec3 E = obb.size; // OBB Extents

		Vec3 A[] = { // OBB Axis
			obb.orientation[0],
			obb.orientation[1],
			obb.orientation[2],
		};

		v[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
		v[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
		v[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
		v[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
		v[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
		v[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
		v[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
		v[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
		
		return v;
	}

	util::Array<Line> GetEdges(const OBB& obb) {
		util::Array<Line> result;
		result.reserve(12);

		util::Array<Point> v = GetVertices(obb);

		int index[][2] = { // Indices of edge-vertices
			{6,1},{6,3},{6,4},{2,7},{2,5},{2,0},
			{0,1},{0,3},{7,1},{7,4},{4,5},{5,3}
		};

		for (int j = 0; j < 12; ++j) {
			result.push_back(Line(
				v[index[j][0]], v[index[j][1]]
			));
		}

		return result;
	}

	util::Array<Plane> GetPlanes(const OBB& obb) {
		Vec3 c = obb.position; // OBB Center
		Vec3 e = obb.size; // OBB Extents

		Vec3 a[] = { // OBB Axis
			obb.orientation[0],
			obb.orientation[1],
			obb.orientation[2],
		};

		util::Array<Plane> result;
		result.resize(6);

		result[0] = Plane( a[0], glm::dot(a[0], (c + a[0] * e.x)));
		result[1] = Plane(-a[0],-glm::dot(a[0], (c - a[0] * e.x)));
		result[2] = Plane( a[1], glm::dot(a[1], (c + a[1] * e.y)));
		result[3] = Plane(-a[1],-glm::dot(a[1], (c - a[1] * e.y)));
		result[4] = Plane( a[2], glm::dot(a[2], (c + a[2] * e.z)));
		result[5] = Plane(-a[2],-glm::dot(a[2], (c - a[2] * e.z)));

		return result;
	}

	bool ClipToPlane(const Plane& plane, const Line& line, Point* outPoint) {
		Vec3 ab = line.end - line.start;
		float nAB = glm::dot(plane.normal, ab);
		if (CMP(nAB, 0)) return false;

		float nA = glm::dot(plane.normal, line.start);
		float t = (plane.distance - nA) / nAB;

		if (t >= 0.0f && t <= 1.0f) {
			if (outPoint != 0) {
				*outPoint = line.start + ab * t;
			}

			return true;
		}

		return false;
	}

	util::Array<Point> ClipEdgesToOBB(const std::vector<Line>& edges, const OBB& obb) {
		util::Array<Point> result;
		result.reserve(edges.size());
		Point intersection;

		util::Array<Plane> planes = GetPlanes(obb);

		for (int i = 0; i < planes.size(); ++i) {
			for (int j = 0; j < edges.size(); ++j) {
				if (ClipToPlane(planes[i], edges[j], &intersection)) {
					if (PointInOBB(intersection, obb)) {
						result.push_back(intersection);
					}
				}
			}
		}

		return result;
	}

	f32 PenetrationDepth(const OBB& o1, const OBB& o2, const Vec3& axis, bool* outShouldFlip) {
		Interval i1 = GetInterval(o1, glm::normalize(axis));
		Interval i2 = GetInterval(o2, glm::normalize(axis));

		if (!((i2.min <= i1.max) && (i1.min <= i2.max))) {
			return 0.0f; // No penerattion
		}

		float len1 = i1.max - i1.min;
		float len2 = i2.max - i2.min;
		float min = fminf(i1.min, i2.min);
		float max = fmaxf(i1.max, i2.max);
		float length = max - min;
		if (outShouldFlip != 0) {
			*outShouldFlip = (i2.min < i1.min);
		}
		return (len1 + len2) - length;
	}

	CollisionManifold FindCollisionFeatures(const OBB& A, const OBB& B) {
		CollisionManifold result;
		ResetCollisionManifold(&result);

		Vec3 test[15] = { // Face axis
			A.orientation[0],
			A.orientation[1],
			A.orientation[2],
			B.orientation[0],
			B.orientation[1],
			B.orientation[2],
		};

		for (int i = 0; i < 3; ++i) { // Fill out rest of axis
			test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
			test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
			test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
		}

		Vec3* hitNormal = 0;
		bool shouldFlip;

		for (int i = 0; i < 15; ++i) {
			if (glm::length2(test[i]) < 0.001f) {
				continue;
			}
			float depth = PenetrationDepth(A, B, test[i], &shouldFlip);
			if (depth <= 0.0f) return result;
			else if (depth < result.depth) {
				if (shouldFlip) {
					test[i] *= -1.0f;
				}
				result.depth = depth;
				hitNormal = &test[i];
			}
		}

		if (hitNormal == 0) {
			return result;
		}
		Vec3 axis = glm::normalize(*hitNormal);
		util::Array<Point> c1 = ClipEdgesToOBB(GetEdges(B), A);
		util::Array<Point> c2 = ClipEdgesToOBB(GetEdges(A), B);
		result.contacts.reserve(c1.size() + c2.size());
		result.contacts.insert(result.contacts.end(), c1.begin(), c1.end());
		result.contacts.insert(result.contacts.end(), c2.begin(), c2.end());

		Interval i = GetInterval(A, axis);
		float distance = (i.max - i.min) * 0.5f - result.depth * 0.5f;
		Vec3 pointOnPlane = A.position + axis * distance;
		for (int i = result.contacts.size() - 1; i >= 0; --i) {
			Vec3 &contact = result.contacts[i];
			result.contacts[i] = contact + (axis * glm::dot(axis, pointOnPlane - contact));
			for (int j = result.contacts.size() - 1; j > i; --j) {
				if (glm::length2(result.contacts[j] - result.contacts[i]) < 0.0001f) {
					result.contacts.erase(result.contacts.begin() + j);
					break;
				}
			}
		}

		result.colliding = true;
		result.normal = axis;
		return result;
	}
}