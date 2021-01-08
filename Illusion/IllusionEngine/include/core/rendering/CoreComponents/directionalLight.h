#pragma once

#include "ecs/Component.h"

namespace illusion::core::rendering {

	using namespace illusion::ecs;

	struct DirectionalLight : public Component {
		DirectionalLight(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(specular);
			COMPONENT_PUBLIC(diffuse);
			COMPONENT_PUBLIC(ambient);
		}

		COMPONENT_NAME("Directional Light");
		COMPONENT_REGISTER(DirectionalLight);

		COMPONENT_DATA(Vec4, specular);
		COMPONENT_DATA(Vec4, ambient);
		COMPONENT_DATA(Vec4, diffuse);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
