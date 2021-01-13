#pragma once

#include "ecs/Component.h"
#include "ecs/CoreComponents/Transform.h"

namespace illusion::ecs::core {

	struct Camera : public Component {
		Camera(Scene* scene);

		COMPONENT_NAME("Camera");
		COMPONENT_REGISTER(Camera);

		COMPONENT_DATA(boolean, editMode);
		COMPONENT_DATA(Vec3, editPosition);

		COMPONENT_DATA(f32, fov);
		COMPONENT_DATA(f32, near);
		COMPONENT_DATA(f32, far);
		COMPONENT_DATA(bool, orthoMode);

		COMPONENT_DATA(Mat4x4, view);
		COMPONENT_DATA(Mat4x4, projection);

		COMPONENT_DATA(Vec4, background);

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

		ecs::core::Transform *transform = nullptr;

		void UpdateRotation(ecs::entity_id id, f32 xoffset, f32 yoffset);
		void UpdateVectors(ecs::entity_id id);
		void UpdatePosition(ecs::entity_id id, f32 translation);
		void UpdatePosition(ecs::entity_id id, Vec2 translation);

		Vec3 GetPosition(ecs::component_id index = (ecs::component_id)0);

		Mat4x4 GetProjection(ecs::component_id index = (ecs::component_id)0);
		Mat4x4 GetView(ecs::component_id index = (ecs::component_id)0);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
