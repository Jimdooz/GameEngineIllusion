#pragma once

#include "ecs/Component.h"
#include "core/animation/Animation.h"

namespace illusion::core::animation {

	using namespace illusion::ecs;

	struct Animator : public Component {
		Animator(Scene* scene) : Component(scene) {
			COMPONENT_PROTECTED(animator);
		}

		COMPONENT_NAME("Animator");
		COMPONENT_REGISTER(Animator);

		COMPONENT_DATA(AnimatorElement, animator);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
