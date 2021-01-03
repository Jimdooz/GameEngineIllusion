#pragma once

#include "./LineIntersections.h"
#include "./PointTests.h"
#include "./ShapeIntersections.h"
#include "./Manifold.h"

inline void DEBUG_LINE(Vec3 p1, Vec3 p2) {
	glBegin(GL_LINES);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glEnd();
}

inline void DEBUG_POINT(Vec3 p1) {
	glBegin(GL_POINTS);
	glVertex3f(p1.x, p1.y, p1.z);
	glEnd();
}
	