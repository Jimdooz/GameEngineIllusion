#pragma once

#include "common/CommonHeaders.h"
#include "./Point.h"

namespace illusion::core::physics::primitives {
	struct OBB {
		Point position;
		Vec3 size;
		Mat3x3 orientation;

		inline OBB() : size(1,1,1) {}
		inline OBB(const Point &p, const Vec3& s) : position(p), size(s) {}
		inline OBB(const Point& p, const Vec3& s, const Mat3x3& o) : position(p), size(s), orientation(o) {}
	};
}