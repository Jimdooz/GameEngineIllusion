#pragma once

#include "common/CommonHeaders.h"
#include "./Point.h"

namespace illusion::core::physics::primitives {
	struct Ray {
		Point origin;
		Vec3 direction;

		inline Ray() : direction(0.0f, 0.0f, 1.0f) {}
		inline Ray(const Point& o, const Vec3& d) : origin(o), direction(d) {
			NormalizeDirection();
		}

		inline void NormalizeDirection() {
			glm::normalize(direction);
		}

		static Ray FromPoints(const Point& from, const Point& to);
	};
}