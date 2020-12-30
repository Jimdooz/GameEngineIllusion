#pragma once

#include "common/CommonHeaders.h"
#include "./Point.h"

namespace illusion::core::physics::primitives {
	struct Sphere {
		Point position;
		f32 radius;

		inline Sphere() : radius(1.0f) {}
		inline Sphere(const Point& p, const f32 r) : position(p), radius(r) {}

	};
}