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

		primitives::Sphere GetSphere(ecs::entity_id id) {
			ecs::component_id index = getIndex(id);
			ecs::core::Transform* transform = scene->GetComponent<ecs::core::Transform>();

			glm::vec3 scale; glm::quat rotation; glm::vec3 translation;
			glm::vec3 skew; glm::vec4 perspective;
			glm::decompose(transform->modelTransform[transform->getIndex(id)], scale, rotation, translation, skew, perspective);
			Vec3 size = Vec3(0.5, 0.5, 0.5);

			return primitives::Sphere(translation, (size * scale).x);
		}

		Mat4x4 InvTensor(ecs::entity_id id, const f32& mass) {
			float ix = 0.0f;
			float iy = 0.0f;
			float iz = 0.0f;
			float iw = 0.0f;

			if (mass != 0) {
				primitives::Sphere sphere = GetSphere(id);
				float r2 = sphere.radius * sphere.radius;
				float fraction = (2.0f / 5.0f);
				ix = r2 * mass * fraction;
				iy = r2 * mass * fraction;
				iz = r2 * mass * fraction;
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