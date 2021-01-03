#pragma once

#include "ecs/Component.h"

using namespace illusion;

struct CubeRenderer : public ecs::Component {
	// Declare component name
	COMPONENT_NAME("Cube Renderer");
	COMPONENT_REGISTER(CubeRenderer);
	// Declare constructor
	CubeRenderer(ecs::Scene* scene) : Component(scene) {}
};