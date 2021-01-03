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
			glm::decompose(transform->ComputeModel(transform->getIndex(id)), scale, rotation, translation, skew, perspective);
			f32 size = 0.5;

			return primitives::OBB(translation, scale * size, glm::toMat4(glm::conjugate(rotation)));
		}

		Mat4x4 InvTensor(ecs::entity_id id, const f32 &mass) {
			float ix = 0.0f;
			float iy = 0.0f;
			float iz = 0.0f;
			float iw = 0.0f;

			if (mass != 0) {
				primitives::OBB box = GetBox(id);
				Vec3 size = box.size * 2.0f;
				float fraction = (1.0f / 12.0f);
				float x2 = size.x * size.x;
				float y2 = size.y * size.y;
				float z2 = size.z * size.z;
				ix = (y2 + z2) * mass * fraction;
				iy = (x2 + z2) * mass * fraction;
				iz = (x2 + y2) * mass * fraction;
				iw = 1.0f;
			}

			return glm::inverse(Mat4x4(
				ix, 0, 0, 0,
				0, iy, 0, 0,
				0, 0, iz, 0,
				0, 0, 0, iw));
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