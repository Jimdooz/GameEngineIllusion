#pragma once

#include "ecs/Component.h"
#include "core/animation/Bone.h"

namespace illusion::core::animation {

	using namespace illusion::ecs;

	struct Skeleton : public Component {

		Skeleton(Scene* scene) : Component(scene) {
			//COMPONENT_PROTECTED(bones); //@Todo Save bones through Json
		}

		COMPONENT_NAME("Skeleton");
		COMPONENT_REGISTER(Skeleton);

		COMPONENT_DATA(util::Array<Bone>, bones);
		COMPONENT_DATA(boolean, idsComputed);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};
}