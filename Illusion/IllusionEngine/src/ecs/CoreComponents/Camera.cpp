#include "ecs/CoreComponents/Camera.h"

namespace illusion::ecs::core {

	void Camera::UpdateRotation(ecs::entity_id id, f32 xoffset, f32 yoffset) {
		ecs::component_id index = getIndex(id);

		xoffset *= mouseSensitivity[index];
		yoffset *= mouseSensitivity[index];

		yaw[index] += xoffset;
		pitch[index] += yoffset;

		if (pitch[index] > 89.5f) pitch[index] = 89.5f;
		if (pitch[index] < -89.5f) pitch[index] = -89.5f;

		UpdateVectors(id);
	}

	void Camera::UpdateVectors(ecs::entity_id id) {
		ecs::component_id index = getIndex(id);

		Vec3 frontE;
		frontE.x = cos(glm::radians(yaw[index])) * cos(glm::radians(pitch[index]));
		frontE.y = sin(glm::radians(pitch[index]));
		frontE.z = sin(glm::radians(yaw[index])) * cos(glm::radians(pitch[index]));
		front[index] = glm::normalize(frontE);

		right[index] = glm::normalize(glm::cross(front[index], worldUp[index]));
		up[index] = glm::normalize(glm::cross(right[index], front[index]));
	}

	void Camera::AddDatas(ecs::entity_id id) {
		AddData(fov, f32(45));
		AddData(near, f32(0.01f));
		AddData(far, f32(1000.0f));
		AddData(orthoMode, false);

		AddData(background, Vec4(0.25f, 0.25f, 0.5f, 1.0));

		AddData(view, Mat4x4(1.0));
		AddData(projection, Mat4x4(1.0));

		AddData(front, glm::vec3(0.0f, 0.0f, -1.0f));
		AddData(up, Vec3(0.0f, 1.0f, 0.0f));
		AddData(right, Vec3(0.0f, 1.0f, 0.0f));
		AddData(worldUp, Vec3(0.0f, 1.0f, 0.0f));
		// euler Angles
		AddData(yaw, -90.0f);
		AddData(pitch, 0.0f);
		// camera options
		AddData(movementSpeed, 10.0f);
		AddData(mouseSensitivity, 0.1f);
		AddData(zoom, 45.0f);
	}

	void Camera::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(fov, index);
		RemoveData(near, index);
		RemoveData(far, index);
		RemoveData(orthoMode, index);

		RemoveData(background, index);

		RemoveData(view, index);
		RemoveData(projection, index);

		RemoveData(front, index);
		RemoveData(up, index);
		RemoveData(right, index);
		RemoveData(worldUp, index);
		RemoveData(yaw, index);
		RemoveData(pitch, index);
		RemoveData(movementSpeed, index);
		RemoveData(mouseSensitivity, index);
		RemoveData(zoom, index);
	}
}
