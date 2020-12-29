#include "ecs/CoreComponents/Camera.h"

namespace illusion::ecs::core {

	void Camera::AddDatas(ecs::entity_id id) {
		AddData(fov, f32(45));
		AddData(near, f32(0.01f));
		AddData(far, f32(1000.0f));
		AddData(orthoMode, false);
	}

	void Camera::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(fov, index);
		RemoveData(near, index);
		RemoveData(far, index);
		RemoveData(orthoMode, index);
	}
}
