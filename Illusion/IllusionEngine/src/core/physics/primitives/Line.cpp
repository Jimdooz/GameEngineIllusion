#include "core/physics/primitives/Line.h"

namespace illusion::core::physics::primitives {
	f32 Line::Length(const Line& line) {
		return glm::length(line.end - line.start);
	}

	f32 Line::LengthSq(const Line& line) {
		return glm::length2(line.end - line.start);
	}
}