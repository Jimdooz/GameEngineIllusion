#pragma once

#include "collisions/Collisions.h"
#include "primitives/Primitives.h"

#include "ecs/Scene.h"

namespace illusion::core::physics {

	void ComputePhysics(ecs::Scene& scene);

	collisions::CollisionManifold FindCollisionFeatures(ecs::Scene& scene, ecs::entity_id A, ecs::entity_id B);

	void ApplyImpulse(ecs::Scene& scene, ecs::entity_id A, ecs::entity_id B, const collisions::CollisionManifold& M, int c);
}