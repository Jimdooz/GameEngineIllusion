#pragma once

#include "common/CommonHeaders.h"
#include "./Point.h"

namespace illusion::core::physics::primitives {

	struct Plane {
		Vec3 normal{};
		float distance{};
		inline Plane() : normal(1, 0, 0) {}
		inline Plane(const Vec3& n, float d) : normal(n), distance(d) {
			glm::normalize(n);
		}

		static f32 PlaneEquation(const Point& pt, const Plane& plane);
	};

}