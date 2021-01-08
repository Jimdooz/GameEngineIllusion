#pragma once

#include "ecs/Component.h"

namespace illusion::core::rendering {

	using namespace illusion::ecs;

	struct PointLight : public Component {
		PointLight(Scene* scene) : Component(scene) {
			COMPONENT_PUBLIC(specular);
			COMPONENT_PUBLIC(diffuse);
			COMPONENT_PUBLIC(ambient);

			COMPONENT_PUBLIC(constant);
			COMPONENT_PUBLIC(linear);
			COMPONENT_PUBLIC(quadratic);
		}

		COMPONENT_NAME("Point Light");
		COMPONENT_REGISTER(PointLight);

		COMPONENT_DATA(f32, constant);
		COMPONENT_DATA(f32, linear);
		COMPONENT_DATA(f32, quadratic);

		COMPONENT_DATA(Vec4, specular);
		COMPONENT_DATA(Vec4, ambient);
		COMPONENT_DATA(Vec4, diffuse);

		virtual void AddDatas(ecs::entity_id id) override;
		virtual void RemoveDatas(ecs::component_id index, ecs::entity_id id) override;
	};

}
