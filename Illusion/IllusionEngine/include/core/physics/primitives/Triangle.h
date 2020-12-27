#pragma once

#include "common/CommonHeaders.h"
#include "./Point.h"

namespace illusion::core::physics::primitives {
	struct Triangle {
		union {
			struct {
				Point a;
				Point b;
				Point c;
			};
			Point points[3];
			float values[9];
		};

		inline Triangle() { }
		inline Triangle(const Point& p1, const Point& p2, const Point& p3) : a(p1), b(p2), c(p3) { }
	};
}