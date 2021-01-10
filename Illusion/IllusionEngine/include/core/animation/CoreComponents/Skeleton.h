#pragma once

#include "ecs/Component.h"
#include "core/animation/Bone.h"

namespace illusion::core::animation {

	using namespace illusion::ecs;

	struct Skeleton : public Component {

		Skeleton(Scene* scene) : Component(scene) {
			COMPONENT_PROTECTED(bones);
		}

		COMPONENT_NAME("Skeleton");
		COMPONENT_REGISTER(Skeleton);

		COMPONENT_DATA(boolean, idsComputed);
		COMPONENT_DATA(std::string, parentRelativePath);
		COMPONENT_DATA(ecs::entity_id, parentId);
		COMPONENT_DATA(util::Array<Bone>, bones);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};
}