#include "core/physics/primitives/Plane.h"

namespace illusion::core::physics::primitives {

	f32 Plane::PlaneEquation(const Point& pt, const Plane& plane) {
		return glm::dot(pt, plane.normal) - plane.distance;
	}

}