#include "core/physics/primitives/Ray.h"

namespace illusion::core::physics::primitives {

	Ray Ray::FromPoints(const Point& from, const Point& to) {
		return Ray(from, to - from);
	}

}