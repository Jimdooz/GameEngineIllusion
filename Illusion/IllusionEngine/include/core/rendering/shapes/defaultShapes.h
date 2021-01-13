#pragma once

#include "core/rendering/Renderer.h"

/**
 * Group all default shapes in the engine , like cube, plane, sphere etc...
 */

namespace illusion::defaultshape {
	namespace {
		Mesh cube;
		Mesh icoSphere;
		Mesh quad;
	}
	void InitializeCube();
	void InitializeIcoSphere();
	void InitializeQuad();
	void Initialize();

	Mesh& Cube();
	Mesh& IcoSphere();
	Mesh& Quad();
}