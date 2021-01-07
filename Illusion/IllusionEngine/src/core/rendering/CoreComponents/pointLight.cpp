#include "core/rendering/CoreComponents/pointLight.h"

namespace illusion::core::rendering {

	void PointLight::AddDatas(ecs::entity_id id) {
		AddData(specular, Vec4(1, 1, 1, 1));
		AddData(ambient, Vec4(0.1, 0.1, 0.1, 0.1));
		AddData(diffuse, Vec4(1, 1, 1, 1));

		AddData(constant, 1.0f);
		AddData(linear, 0.09f);
		AddData(quadratic, 0.032f);
	}

	void PointLight::RemoveDatas(ecs::component_id index, ecs::entity_id id) {
		RemoveData(index, specular, ambient, diffuse);
		RemoveData(index, constant, linear, quadratic);
	}

}
