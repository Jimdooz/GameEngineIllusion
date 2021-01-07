#include "core/rendering/CoreComponents/directionalLight.h"

namespace illusion::core::rendering {

	void DirectionalLight::AddDatas(ecs::entity_id id) {
		AddData(specular,  Vec4(1, 1, 1, 1));
		AddData(ambient, Vec4(0.1, 0.1, 0.1, 0.1));
		AddData(diffuse, Vec4(1, 1, 1, 1));
	}

	void DirectionalLight::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, specular, ambient, diffuse);
	}

}
