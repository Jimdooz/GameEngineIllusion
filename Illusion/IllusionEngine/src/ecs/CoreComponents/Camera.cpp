#include "ecs/CoreComponents/Camera.h"
#include "ecs/Scene.h"
#include "Window.h"

namespace illusion::ecs::core {
	Camera::Camera(Scene* scene) : Component(scene) {
		COMPONENT_PUBLIC(background);
		COMPONENT_PUBLIC(editMode);
		COMPONENT_PROTECTED(editPosition);

		COMPONENT_PUBLIC(fov);
		COMPONENT_PUBLIC(orthoMode);

		COMPONENT_PUBLIC(yaw);
		COMPONENT_PUBLIC(pitch);

		INTERNAL_INFO("TRY TRANSFORM GET");
		INTERNAL_INFO("TRANSFORM GET");
	}

	void Camera::UpdateRotation(ecs::entity_id id, f32 xoffset, f32 yoffset) {
		ecs::component_id index = getIndex(id);
		if (!editMode[index]) return;

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

	void Camera::UpdatePosition(ecs::entity_id id, f32 translation) {
		ecs::component_id index = getIndex(id);
		if (!editMode[index]) return;
		editPosition[index] += front[index] * movementSpeed[index] * translation;
	}

	void Camera::UpdatePosition(ecs::entity_id id, Vec2 translation) {
		ecs::component_id index = getIndex(id);
		if (!editMode[index]) return;
		editPosition[index] += right[index] * translation.x + up[0] * translation.y;
	}

	Mat4x4 Camera::GetProjection(ecs::component_id index) {
		float aspect = (float)Window::width / (float)Window::height;
		return glm::perspective(fov[index], aspect, near[index], far[index]);
	}

	Mat4x4 Camera::GetView(ecs::component_id index) {
		//Compute Camera Position
		ecs::entity_id cameraId = getId(index);

		if (editMode[index]) {
			return glm::lookAt(editPosition[index], editPosition[index] + front[index], up[index]);
		}

		ecs::component_id cameraIdTransform = transform->getIndex(cameraId);
		Mat4x4 modelCamera = transform->ComputeModel(cameraIdTransform);
		Vec3 cameraWorldPos(modelCamera[3][0], modelCamera[3][1], modelCamera[3][2]);

		return glm::inverse(modelCamera);
	}

	Vec3 Camera::getPosition(ecs::component_id index) {
		ecs::entity_id cameraId = getId(index);
		if (editMode[index]) return editPosition[index];
		ecs::component_id cameraIdTransform = transform->getIndex(cameraId);
		Mat4x4 modelCamera = transform->ComputeModel(cameraIdTransform);
		Vec3 cameraWorldPos(modelCamera[3][0], modelCamera[3][1], modelCamera[3][2]);
		return cameraWorldPos;
	}

	void Camera::AddDatas(ecs::entity_id id) {
		if(transform == nullptr) transform = scene->GetComponent<ecs::core::Transform>();
		AddData<boolean>(editMode, true);
		AddData(editPosition, Vec3(0.0f, 0.0f, 0.0f));

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
		RemoveData(index, editMode, editPosition);

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
