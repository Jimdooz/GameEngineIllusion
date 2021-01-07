#include "core/rendering/CoreComponents/directionalLight.h"

namespace illusion::core::rendering {

	void DirectionalLight::AddDatas(ecs::entity_id id) {
		AddData(color, Vec4(1, 1, 1, 1));
	}

	void DirectionalLight::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, color);
	}

}
