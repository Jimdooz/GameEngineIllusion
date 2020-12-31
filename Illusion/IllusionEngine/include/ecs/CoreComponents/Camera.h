#pragma once

#include "ecs/Component.h"

namespace illusion::ecs::core {

	struct Camera : public Component {
		Camera(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(fov);
			COMPONENT_PUBLIC(near);
			COMPONENT_PUBLIC(far);
			COMPONENT_PUBLIC(orthoMode);

			COMPONENT_PROTECTED(yaw);
			COMPONENT_PROTECTED(pitch);
		}

		COMPONENT_NAME("Camera");
		COMPONENT_REGISTER(Camera);

		COMPONENT_DATA(f32, fov);
		COMPONENT_DATA(f32, near);
		COMPONENT_DATA(f32, far);
		COMPONENT_DATA(bool, orthoMode);

		COMPONENT_DATA(Mat4x4, view);
		COMPONENT_DATA(Mat4x4, projection);

		//Attributes
		COMPONENT_DATA(Vec3, front);
		COMPONENT_DATA(Vec3, up);
		COMPONENT_DATA(Vec3, right);
		COMPONENT_DATA(Vec3, worldUp);
		// euler Angles
		COMPONENT_DATA(f32, yaw);
		COMPONENT_DATA(f32, pitch);
		// camera options
		COMPONENT_DATA(f32, movementSpeed);
		COMPONENT_DATA(f32, mouseSensitivity);
		COMPONENT_DATA(f32, zoom);

		void UpdateRotation(ecs::entity_id id, f32 xoffset, f32 yoffset);
		void UpdateVectors(ecs::entity_id id);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
