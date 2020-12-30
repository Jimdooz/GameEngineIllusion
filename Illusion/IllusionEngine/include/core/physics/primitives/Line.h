#pragma once

#include "common/CommonHeaders.h"
#include "./Point.h"

namespace illusion::core::physics::primitives {
	struct Line {
		Point start;
		Point end;

		inline Line(){}
		inline Line(const Point& s, const Point& e) : start(s), end(e) {}

		static f32 Length(const Line& line);
		static f32 LengthSq(const Line& line);
	};
}