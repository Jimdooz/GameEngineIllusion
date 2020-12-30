#pragma once

#include "ecs/Component.h"

namespace illusion::core::physics {

	using namespace illusion;

	struct SphereCollider : public ecs::Component {
		COMPONENT_NAME("Sphere Collider");
		COMPONENT_REGISTER(SphereCollider);

		SphereCollider(ecs::Scene* scene) : Component(scene) {}

		COMPONENT_DATA(bool, trigger);
		COMPONENT_DATA(util::Array<ecs::entity_id>, collisions);

		virtual void AddDatas(ecs::entity_id id) override {
			AddData(trigger, false);
			AddData(collisions, util::Array<ecs::entity_id>());
		}

		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) {
			RemoveData(trigger, index);
			RemoveData(collisions, index);
		}
	};

}