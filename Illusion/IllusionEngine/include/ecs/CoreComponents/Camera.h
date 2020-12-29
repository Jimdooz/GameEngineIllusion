#pragma once

#include "ecs/Component.h"

namespace illusion::ecs::core {

	struct Camera : public Component {
		Camera(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(fov);
			COMPONENT_PUBLIC(near);
			COMPONENT_PUBLIC(far);
			COMPONENT_PUBLIC(orthoMode);
		}

		COMPONENT_NAME("Camera");
		COMPONENT_REGISTER(Camera);

		COMPONENT_DATA(f32, fov);
		COMPONENT_DATA(f32, near);
		COMPONENT_DATA(f32, far);
		COMPONENT_DATA(bool, orthoMode);

		virtual void AddDatas(ecs::entity_id id) override;

		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id);
	};

}
