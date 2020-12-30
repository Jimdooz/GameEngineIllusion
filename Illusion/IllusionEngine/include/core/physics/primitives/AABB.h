#pragma once

#include "common/CommonHeaders.h"
#include "./Point.h"

namespace illusion::core::physics::primitives {
	struct AABB {
		Point position;
		Vec3 size;

		inline AABB() : size(1, 1, 1) { }
		inline AABB(const Point& p, const Vec3& s) : position(p), size(s) { }

		static Vec3 GetMin(const AABB& aabb);
		static Vec3 GetMax(const AABB& aabb);
		static AABB FromMinMax(const Vec3& min, const Vec3& max);
	};
}