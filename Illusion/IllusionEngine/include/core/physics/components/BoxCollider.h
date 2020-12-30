#pragma once

#include "ecs/Component.h"
#include "core/physics/primitives/Primitives.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace illusion::core::physics {

	using namespace illusion;

	struct BoxCollider : public ecs::Component {
		COMPONENT_NAME("Box Collider");
		COMPONENT_REGISTER(BoxCollider);

		BoxCollider(ecs::Scene* scene) : Component(scene) {}

		COMPONENT_DATA(bool, trigger);
		COMPONENT_DATA(util::Array<ecs::entity_id>, collisions);

		primitives::OBB GetBox(ecs::entity_id id) {
			ecs::component_id index = getIndex(id);
			ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();

			glm::vec3 scale; glm::quat rotation; glm::vec3 translation;
			glm::vec3 skew; glm::vec4 perspective;
			glm::decompose(transform->modelTransform[transform->getIndex(id)], scale, rotation, translation, skew, perspective);
			Vec3 size = Vec3(0.5, 0.5, 0.5);

			return primitives::OBB(translation, scale * size, glm::inverse(glm::toMat4(rotation)));
		}

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